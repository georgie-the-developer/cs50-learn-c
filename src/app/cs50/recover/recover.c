#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Define the BYTE type
typedef uint8_t BYTE;

int recover(int argc, char *argv[])
{
    printf("JPEG recovery program\n");

    // Check args
    if (argc != 3)
    {
        printf("Usage: %s <raw_file.raw> <dir_to_store_files_to/>\n", argv[0]);
        return 1;
    }

    // Open the raw file
    FILE *raw = fopen(argv[1], "rb");
    if (raw == NULL)
    {
        printf("Failed to open file %s\n", argv[1]);
        return 1;
    }

    BYTE buf[512];     // allocate a 512-byte buffer
    FILE *curr = NULL; // declare a current file pointer
    int fI = 0;        // initialize the file index (for naming)
    // read from file
    while (fread(buf, 1, 512, raw) != 0)
    {
        // search for jpeg signatures
        if (buf[0] == 0xff && buf[1] == 0xd8 && buf[2] == 0xff && (buf[3] - buf[3] % 16) / 16 == 0xe)
        {
            if (curr != NULL)
                fclose(curr); // close the current file

            // get filename from index
            char *filename = malloc(sizeof(char) * 4);
            int fICpy = fI; // copy the index (in order not to overwrite the original one)
            for (int j = 2; j >= 0; j--)
            {
                if (fICpy > 1)
                {
                    int digit = fICpy % 10;
                    filename[j] = '0' + digit;
                    fICpy = fICpy / 10;
                }
                else
                {
                    filename[j] = '0';
                }
            }
            filename[4] = '\0'; // null-terminate filename

            // get full filename
            int fullFilenameSize = strlen(argv[2]) + strlen(filename) + 5;
            char *fullFilename = malloc(sizeof(char) * fullFilenameSize);
            sprintf(fullFilename, "%s%s.jpg", argv[2], filename);
            fullFilename[strlen(fullFilename)] = '\0'; // null-terminate fullFilename

            curr = fopen(fullFilename, "wb"); // open (create) a file

            fI++; // increment the file index

            // free malloc-ed values
            free(filename);
            free(fullFilename);
        }
        if (curr != NULL)
        {
            fwrite(buf, 1, 512, curr); // write buffer to files
        }
    }

    return 0;
}