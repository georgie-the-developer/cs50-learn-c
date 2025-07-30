#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "./inheritance.h"

int inheritance(int argc, char *argv[])
{
    printf("Blood type inheritance simulation\n");

    if (argc > 2)
    {
        printf("Usage: %s <generations_count(optional)>\n", argv[0]);
        return 1;
    }

    srand(time(NULL));
    person *root;
    if (argc == 1)
    {
        root = createFamily(3);
    }
    else
    {
        root = createFamily(atoi(argv[1]));
    }

    freeFamily(root, 0);

    return 0;
}

person *createFamily(int generations)
{
    int firstGenCount = pow(2, generations - 1);
    return generatePeople(firstGenCount / 2, generateFirstGeneration(firstGenCount));
}
person **generateFirstGeneration(int count)
{
    person **firstGeneration = malloc(sizeof(person *) * count);
    for (int i = 0; i < count; i++)
    {
        person *human = malloc(sizeof(person));
        human->parents[0] = NULL;
        human->parents[1] = NULL;
        human->alleles[0] = randomAllele();
        human->alleles[1] = randomAllele();
        firstGeneration[i] = human;
    }
    return firstGeneration;
}
person *generatePeople(int count, person **currentGeneration)
{
    if (count == 1)
    {
        // Malloc a person
        person *root = malloc(sizeof(person));
        // Assign parents by a clever formula
        root->parents[0] = currentGeneration[0];
        root->parents[1] = currentGeneration[1];
        // Assign alleles at random
        free(currentGeneration);
        for (int j = 0; j < 2; j++)
            root->alleles[j] = root->parents[rand() % 2]->alleles[rand() % 2];

        return root;
    }
    else
    {
        person **nextGeneration = malloc(sizeof(person *) * count);
        for (int i = 0; i < count; i++)
        {
            // Malloc a person
            person *human = malloc(sizeof(person));
            // Assign parents by a clever formula
            human->parents[0] = currentGeneration[i * 2];
            human->parents[1] = currentGeneration[i * 2 + 1];
            // Assign alleles at random
            for (int j = 0; j < 2; j++)
                human->alleles[j] = human->parents[rand() % 2]->alleles[rand() % 2];

            nextGeneration[i] = human;
        }
        free(currentGeneration);
        return generatePeople(count / 2, nextGeneration);
    }
}

void freeFamily(person *human, int genCount)
{
    if (genCount == 0)
    {
        printf("Child (Generation 0): Blood Type %s \n", human->alleles);
    }
    else if (genCount == 1)
    {
        printf(" Parent (Generation 1): Blood Type %s \n", human->alleles);
    }
    else
    {
        char *spaces = malloc(sizeof(char) * (genCount + 1));
        for (int i = 0; i < genCount; i++)
        {
            spaces[i] = ' ';
        }
        spaces[genCount] = '\0';
        printf("%s%sGrandparent (Generation %i): Blood Type %s \n", spaces, spaces, genCount, human->alleles);
    }
    if (human->parents[0] != NULL)
        freeFamily(human->parents[0], genCount + 1);

    if (human->parents[1] != NULL)
        freeFamily(human->parents[1], genCount + 1);
    free(human);
}
char randomAllele()
{
    int i = rand() % 3;
    switch (i)
    {
    case 0:
        return 'A';
    case 1:
        return 'B';
    case 2:
        return 'O';
    default:
        return 'A';
    }
}