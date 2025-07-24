#include "./array_sorter.h"
#include "../../libs/cs50/cs50.h"
#include <stdio.h>
/*
Usage: <program_name> <file_to_seed.csv> <file_to_sort_into.csv>
*/
int arraySorter(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: <%s> <file_to_seed> <file_to_sort_to>\n", argv[0]);
        return 1;
    }
    // Declare limiting constants
    const int MAX_ENTRIES = 1000000;
    const int MAX_ENTRY_LEN = 100;

    // Prompt user for entries count and entry length
    int entryCount = getEntryCount(MAX_ENTRIES);
    int entryLen = getEntryLen(MAX_ENTRY_LEN);

    // Seed the src file
    int res = seed(entryCount, entryLen, argv[1]);
    if (res == 1)
    {
        printf("Error seeding file %s \n", argv[1]);
        return 1;
    }
    // Sort from src to dest file
    res = sort(argv[1], argv[2], entryLen, 's');
    if (res == 1)
    {
        printf("Error sorting from file %s to file %s \n", argv[1], argv[2]);
        return 1;
    }

    printf("Operation successful! \n");
    return 0;
}

// Prompt the user for entryCount
int getEntryCount(int maxEntries)
{
    while (true)
    {
        int entryCount = get_int("How much entries should there be? ");
        if (entryCount > 0 && entryCount <= maxEntries)
        {
            return entryCount;
        }
        else
        {
            printf("There can be no more than %i entries; the number can't be negative\n", maxEntries);
        }
    }
}

// Prompt the user for entryLen
int getEntryLen(int maxEntryLen)
{
    while (true)
    {
        int entryLen = get_int("How long should each entry be? ");
        if (entryLen > 0 && entryLen <= maxEntryLen)
        {
            return entryLen;
        }
        else
        {
            printf("The entry can have no more than %i elements; the number can't be negative\n", maxEntryLen);
        }
    }
}