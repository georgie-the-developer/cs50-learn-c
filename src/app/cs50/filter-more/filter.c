#include <stdio.h>
#include "./filter.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef uint8_t BYTE;

int IMAGE_HEIGHT;
int IMAGE_WIDTH;
int IMAGE_WIDTH_PADDING;
// Entry point
int filter(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: %s -filter_mode <infile> <outfile> <blur-intensity(optional)>\n", argv[0]);
        return 1;
    }
    FILE *infile = fopen(argv[2], "rb");
    FILE *outfile = fopen(argv[3], "wb");
    if (infile == NULL)
    {
        perror("Couldn't open infile\n");
        return 1;
    }
    if (outfile == NULL)
    {
        perror("Failed to create outfile\n");
        return 1;
    }

    BYTE BMFileHeader[14];
    BYTE BMInfoHeader[40];

    // Copy file header
    fread(BMFileHeader, sizeof(BMFileHeader), 1, infile);
    fwrite(BMFileHeader, sizeof(BMFileHeader), 1, outfile);

    // Get the info header
    fread(BMInfoHeader, sizeof(BMInfoHeader), 1, infile);

    // Get the width and the height
    memcpy(&IMAGE_WIDTH, &BMInfoHeader[4], sizeof(int));
    memcpy(&IMAGE_HEIGHT, &BMInfoHeader[8], sizeof(int));
    // Write the info header to the outfile
    fwrite(BMInfoHeader, sizeof(BMInfoHeader), 1, outfile);

    enum FilterMode
    {
        REFLECT = 'r',
        GRAYSCALE = 'g',
        BLUR = 'b',
        EDGES = 'e'
    };
    int res;

    switch (argv[1][1])
    {
    case GRAYSCALE:
        res = grayscale(infile, outfile);
        break;
    case REFLECT:
        res = reflect(infile, outfile);
        break;
    case BLUR:
        if (argc == 5)
            res = blur(infile, outfile, atoi(argv[4]));
        else
            res = blur(infile, outfile, 1);
        break;
    case EDGES:
        res = edges(infile, outfile);
        break;
    default:
        printf("Unknown image modifier\n");
        res = 1;
        break;
    }

    return res;
}

// Filters
int grayscale(FILE *infile, FILE *outfile)
{
    printf("Grayscale\n");

    BYTE buffer[3];
    // Change the bitmap
    while (fread(buffer, sizeof(buffer), 1, infile) != 0)
    {
        if (ferror(infile))
        {
            printf("Read error occured\n");
            return 1;
        }

        BYTE average = (buffer[0] + buffer[1] + buffer[2]) / 3; // get the average of all pixels

        // write 3 averaged bytes to the outfile
        fwrite(&average, 1, 1, outfile);
        fwrite(&average, 1, 1, outfile);
        fwrite(&average, 1, 1, outfile);
        if (ferror(outfile))
        {
            printf("Write error occured\n");
            return 1;
        }
    }
    return 0;
}

int reflect(FILE *infile, FILE *outfile)
{
    printf("Reflect\n");

    int len = IMAGE_WIDTH * 3;
    BYTE *buffer = malloc(1 * len);
    // Read each line
    while (fread(buffer, 1, len, infile) != 0)
    {
        // Swap pixels in place
        for (int i = 0; i < IMAGE_WIDTH / 2; i++)
        {
            int leftStart = i * 3;
            int rightStart = (IMAGE_WIDTH - 1 - i) * 3;
            for (int j = 0; j < 3; j++)
            {
                BYTE left = buffer[leftStart + j];
                buffer[leftStart + j] = buffer[rightStart + j];
                buffer[rightStart + j] = left;
            }
        }
        // Write to outfile
        fwrite(buffer, 1, len, outfile);

        // Copy the padding
        BYTE pad[IMAGE_WIDTH_PADDING];
        fread(pad, 1, IMAGE_WIDTH_PADDING, infile);
        fwrite(pad, 1, IMAGE_WIDTH_PADDING, outfile);
    }
    printf("\n");
    free(buffer);
    return 0;
}

int blur(FILE *infile, FILE *outfile, int radius)
{
    printf("Blur\n");

    // Pixel struct
    struct Pixel
    {
        BYTE b;
        BYTE g;
        BYTE r;
    };
    // Get a two-dimentional bitmap from the file
    struct Pixel **bitmap = malloc(sizeof(struct Pixel *) * IMAGE_HEIGHT);
    BYTE pad[IMAGE_WIDTH_PADDING];
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        bitmap[i] = malloc(IMAGE_WIDTH * sizeof(struct Pixel));
        fread(bitmap[i], sizeof(struct Pixel), IMAGE_WIDTH, infile);
        // Copy the padding
        fread(pad, 1, IMAGE_WIDTH_PADDING, infile);
    }

    // Average each neigbouring pixel
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            int rSum = 0;
            int gSum = 0;
            int bSum = 0;
            int neighbours = 0;
            for (int k = j - radius; k <= j + radius; k++)
                if (k >= 0)
                {
                    if (k >= IMAGE_WIDTH)
                        break;

                    for (int l = i - radius; l <= i + radius; l++)
                        if (l >= 0)
                        {
                            if (l >= IMAGE_HEIGHT)
                                break;
                            rSum += bitmap[l][k].r;
                            gSum += bitmap[l][k].g;
                            bSum += bitmap[l][k].b;
                            neighbours++;
                        }
                }

            struct Pixel currentP = bitmap[i][j];
            currentP.r = (BYTE)(rSum / neighbours);
            currentP.g = (BYTE)(gSum / neighbours);
            currentP.b = (BYTE)(bSum / neighbours);
            bitmap[i][j] = currentP;
            fwrite(&currentP, 3, 1, outfile);
        }
        fwrite(pad, 1, IMAGE_WIDTH_PADDING, outfile);
    }
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        free(bitmap[i]);
    }
    free(bitmap);

    return 0;
}

int edges(FILE *infile, FILE *outfile)
{
    printf("Edges\n");

    // Pixel struct
    struct Pixel
    {
        BYTE b;
        BYTE g;
        BYTE r;
    };
    // Get a two-dimentional bitmap from the file
    struct Pixel **bitmap = malloc(sizeof(struct Pixel *) * IMAGE_HEIGHT);
    BYTE pad[IMAGE_WIDTH_PADDING];
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        bitmap[i] = malloc(IMAGE_WIDTH * sizeof(struct Pixel));
        fread(bitmap[i], sizeof(struct Pixel), IMAGE_WIDTH, infile);
        // Copy the padding
        fread(pad, 1, IMAGE_WIDTH_PADDING, infile);
    }

    // Apply the edge-detection algo
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            int rGx = 0;
            int rGy = 0;
            int gGx = 0;
            int gGy = 0;
            int bGx = 0;
            int bGy = 0;
            if (i - 1 >= 0)
            {
                if (j - 1 >= 0)
                {
                    // Get Gx
                    rGx += bitmap[i - 1][j - 1].r * -1;
                    gGx += bitmap[i - 1][j - 1].g * -1;
                    bGx += bitmap[i - 1][j - 1].b * -1;
                    // Get Gy
                    rGy += bitmap[i - 1][j - 1].r * -1;
                    gGy += bitmap[i - 1][j - 1].g * -1;
                    bGy += bitmap[i - 1][j - 1].b * -1;
                }
                // Get Gy
                rGy += bitmap[i - 1][j].r * -2;
                gGy += bitmap[i - 1][j].g * -2;
                bGy += bitmap[i - 1][j].b * -2;
                if (j + 1 < IMAGE_WIDTH)
                {
                    // Get Gx
                    rGx += bitmap[i - 1][j + 1].r * 1;
                    gGx += bitmap[i - 1][j + 1].g * 1;
                    bGx += bitmap[i - 1][j + 1].b * 1;
                    // Get Gy
                    rGy += bitmap[i - 1][j + 1].r * -1;
                    gGy += bitmap[i - 1][j + 1].g * -1;
                    bGy += bitmap[i - 1][j + 1].b * -1;
                }
            }
            if (j - 1 >= 0)
            {
                // Get Gx
                rGx += bitmap[i][j - 1].r * -2;
                gGx += bitmap[i][j - 1].g * -2;
                bGx += bitmap[i][j - 1].b * -2;
            }
            if (j + 1 < IMAGE_WIDTH)
            {
                // Get Gx
                rGx += bitmap[i][j + 1].r * 2;
                gGx += bitmap[i][j + 1].g * 2;
                bGx += bitmap[i][j + 1].b * 2;
            }
            if (i + 1 < IMAGE_HEIGHT)
            {
                if (j - 1 >= 0)
                {
                    // Get Gx
                    rGx += bitmap[i + 1][j - 1].r * -1;
                    gGx += bitmap[i + 1][j - 1].g * -1;
                    bGx += bitmap[i + 1][j - 1].b * -1;
                    // Get Gy
                    rGy += bitmap[i + 1][j - 1].r * 1;
                    gGy += bitmap[i + 1][j - 1].g * 1;
                    bGy += bitmap[i + 1][j - 1].b * 1;
                }
                // Get Gy
                rGy += bitmap[i + 1][j].r * 2;
                gGy += bitmap[i + 1][j].g * 2;
                bGy += bitmap[i + 1][j].b * 2;
                if (j + 1 < IMAGE_WIDTH)
                {
                    // Get Gx
                    rGx += bitmap[i + 1][j + 1].r * 1;
                    gGx += bitmap[i + 1][j + 1].g * 1;
                    bGx += bitmap[i + 1][j + 1].b * 1;
                    // Get Gy
                    rGy += bitmap[i + 1][j + 1].r * 1;
                    gGy += bitmap[i + 1][j + 1].g * 1;
                    bGy += bitmap[i + 1][j + 1].b * 1;
                }
            }
            struct Pixel currentP;
            BYTE r = sqrt(pow(rGx, 2) + pow(rGy, 2));
            if (r > 255)
                r = 255;
            currentP.r = r;
            BYTE g = sqrt(pow(gGx, 2) + pow(gGy, 2));
            if (g > 255)
                g = 255;
            currentP.g = g;
            BYTE b = sqrt(pow(bGx, 2) + pow(bGy, 2));
            if (b > 255)
                b = 255;
            currentP.b = b;
            fwrite(&currentP, 3, 1, outfile);
        }
        fwrite(pad, 1, IMAGE_WIDTH_PADDING, outfile);
    }
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        free(bitmap[i]);
    }
    free(bitmap);

    return 0;
}