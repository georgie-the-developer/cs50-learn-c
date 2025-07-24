#include "../array_sorter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// First arg is the ammount of entries that have to be seeded to a file
// second arg is the length of each entry
// third is the name of the file that will be seeded
int seed(int qtty, int len, char *filename)
{
    // Open file to seed
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    // Seed randomizer
    srand(time(NULL));

    // Add rows
    for (int i = 0; i < qtty; i++)
    {
        // Add individual elements
        for (int j = 0; j < len; j++)
        {
            int num = rand() % (len * 5 + 1);
            char *el = malloc(sizeof(int));
            sprintf(el, "%i", num);
            size_t inputSize = sizeof(char) * strlen(el);
            fwrite(el, inputSize, 1, file);
            if (j != len - 1)
                fwrite(",", sizeof(char), 1, file);
            free(el);
        }
        fwrite("\n", sizeof(char), 1, file);
    }
    // Close the file
    fclose(file);
    printf("Seeding to file %s successful\n", filename);
    return 0;
}