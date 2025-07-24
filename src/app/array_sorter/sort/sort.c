#include "../array_sorter.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int sort(char *srcFilename, char *destFilename, int entryLen, char sortType)
{
    FILE *src = fopen(srcFilename, "r");
    FILE *dest = fopen(destFilename, "w");

    if (src == NULL)
    {
        printf("Error opening file %s", srcFilename);
        return 1;
    }
    else if (dest == NULL)
    {
        printf("Error opening file %s", destFilename);
        return 1;
    }

    clock_t start, end;
    double cpu_time_used;

    start = clock();
    // read each line
    size_t sizeofLine = entryLen * sizeof(int) + entryLen;
    char buffer[sizeofLine];
    for (int i = 1; fgets(buffer, sizeof(buffer), src) != NULL; i++)
    {
        // Split the buffer into an array of int
        char *token; // represents the current split element
        int *entries = calloc(entryLen, sizeof(int));

        token = strtok(buffer, ",");

        for (int j = 0; token != NULL; j++)
        {
            entries[j] = atoi(token);
            token = strtok(NULL, ",");
        }

        // sort data
        switch (sortType)
        {
        case 's':
            selectionSort(entries, entryLen);
            break;
        case 'b':
            bubbleSort(entries, entryLen);
            break;
        case 'q':
            quickSort(entries, entryLen);
        case 'm':
            mergeSort(entries, entryLen);
        default:
            selectionSort(entries, entryLen);
            break;
        }

        // Write data to the dest
        for (int j = 0; j < entryLen; j++)
        {
            char *sortedEl = malloc(sizeof(int));
            sprintf(sortedEl, "%i", entries[j]);
            size_t inputSize = sizeof(char) * strlen(sortedEl);
            fwrite(sortedEl, inputSize, 1, dest);
            free(sortedEl);
            if (j != entryLen - 1)
                fwrite(",", sizeof(char), 1, dest);
        }
        free(entries);
        fwrite("\n", sizeof(char), 1, dest);
    }
    if (feof(src))
    {
        printf("End of file reached.\n");
    }
    else if (ferror(src))
    {
        printf("An error occured.\n");
        return 1;
    }

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000; // in miliseconds
    printf("Sort successful, time taken: %f ms\n", cpu_time_used);

    fclose(src);
    fclose(dest);

    return 0;
}

int selectionSort(int *entries, int entryLen)
{
    for (int i = 0; i < entryLen; i++)
        for (int j = 0; j < entryLen; j++)
            if (i < j && entries[i] > entries[j])
            {
                int smaller = entries[j];
                entries[j] = entries[i];
                entries[i] = smaller;
            }
}

int bubbleSort(int *entries, int entryLen)
{
}

int quickSort(int *entries, int entryLen)
{
}

int mergeSort(int *entries, int entryLen)
{
}
