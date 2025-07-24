#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int volume(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <input_file.wav> <output_file.wav> <factor>\n", argv[0]);
        return 1;
    }

    // Open input and output files (in binary mode)
    FILE *input = fopen(argv[1], "rb");
    FILE *output = fopen(argv[2], "wb");
    if (input == NULL || output == NULL)
    {
        perror("Could not open one of the given files\n");
        return 1;
    }

    // Copy the header
    uint8_t b;
    const int WAV_HEADER_SIZE = 44;
    for (int i = 0; i < WAV_HEADER_SIZE && fread(&b, sizeof(b), 1, input) != 0; i++)
    {
        if (ferror(input))
        {
            printf("An error occured while reading from %s\n", argv[1]);
            return 1;
        }

        fwrite(&b, sizeof(b), 1, output);

        if (ferror(output))
        {
            printf("An error occured while writing to %s\n", argv[2]);
            return 1;
        }
    }

    // Multiply each 16-bit sample by the factor
    double factor = atof(argv[3]); // get the factor
    int16_t sample;                // intitialize sample container
    while (fread(&sample, sizeof(sample), 1, input) != 0)
    {
        if (ferror(input))
        {
            printf("An error occured while reading from %s\n", argv[1]);
            return 1;
        }

        sample = sample * factor;
        // Handle sample overflow
        if (sample > INT16_MAX)
            sample = INT16_MAX;
        if (sample < INT16_MIN)
            sample + INT16_MIN;

        // write to the output file
        fwrite(&sample, sizeof(sample), 1, output);

        if (ferror(output))
        {
            printf("An error occured while writing to %s\n", argv[2]);
            return 1;
        }
    }

    // Close files
    fclose(input);
    fclose(output);

    return 0;
}