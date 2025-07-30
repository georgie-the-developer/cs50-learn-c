#ifndef INHERITANCE
#define INHERITANCE
int inheritance(int argc, char *argv[]);
char randomAllele();

typedef struct person
{
    struct person *parents[2];
    char alleles[2];
} person;

person *createFamily(int generations);
person **generateFirstGeneration(int count);
person *generatePeople(int count, person **generation);
void freeFamily(person *human, int genCount);
#endif