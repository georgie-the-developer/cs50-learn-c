#ifndef ARRAY_SORTER
#define ARRAY_SORTER
// array_sorter.c
int arraySorter(int argc, char *argv[]);
int getEntryCount(int maxEntries);
int getEntryLen(int maxEntryLen);

// seed.c
int seed(int qtty, int len, char *filename);

// sort.c
int sort(char *srcFilename, char *destFilename, int entryLen, char sortType);
int selectionSort(int *entries, int entryLen);
int bubbleSort(int *entries, int entryLen);
int quickSort(int *entries, int entryLen);
int mergeSort(int *entries, int entryLen);
#endif