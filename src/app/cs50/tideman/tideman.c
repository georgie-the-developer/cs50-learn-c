#include "./tideman.h"
#include "../../../libs/cs50/cs50.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Declare consts
const int MIN_VOTES = 3;
const int MIN_CANDIDATES = 2;
const int MAX_CANDIDATES = 10;

// Return the name of the winner
int pairCount = 0;

// MAIN FUNCTIONS
char *runElection(int candidateCount, char **candidates)
{
    // Check for errors
    if (candidateCount < MIN_CANDIDATES)
    {
        printf("Minimum number of candidates: %i \n", MIN_CANDIDATES);
        exit(1);
    }
    else if (candidateCount > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates (%i) exceeded \n", MAX_CANDIDATES);
        exit(1);
    }

    // Prompt the user for the number of votes
    int voteCount = numOfVotes();

    // Run the election
    // int **votes = vote(voteCount, candidateCount, candidates);

    // Run the test election
    int **votes = voteTest(voteCount, candidateCount, candidates);

    // // Display vote ballots (dev-only)
    // printf("\f");
    // for (int i = 0; i < voteCount; i++)
    // {
    //     printf("Voter: %i \n", i);
    //     for (int j = 0; j < candidateCount; j++)
    //         printf(" Position %i: %s\n", j, candidates[votes[i][j]]);
    // }

    // Get preferences
    int **preferences = getPreferences(voteCount, candidateCount, votes);

    // Display preferences (dev-only)
    printf("\f");
    for (int i = 0; i < candidateCount; i++)
        for (int j = 0; j < candidateCount; j++)
            if (i != j)
                printf("%s over %s: %i\n", candidates[i], candidates[j], preferences[i][j]);

    // Get pairs
    struct pair *pairs = getPairs(preferences, candidateCount);

    // Display unsorted pairs (dev-only)
    printf("\fUnsorted pairs:\n");
    for (int i = 0; i < pairCount; i++)
        printf(" Pair %i, winner: %s, looser: %s, winner prefered over looser %i times \n", i, candidates[pairs[i].winner], candidates[pairs[i].looser], pairs[i].winnerPreferences);

    // Sort pairs
    sortPairs(pairs);

    // Display sorted pairs (dev-only)
    printf("\fSorted pairs:\n");
    for (int i = 0; i < pairCount; i++)
        printf(" Pair %i, winner: %s, looser: %s, winner prefered over looser %i times \n", i, candidates[pairs[i].winner], candidates[pairs[i].looser], pairs[i].winnerPreferences);

    // Lock pairs
    bool **locked = lockPairs(pairs, candidateCount);

    // Display locked pairs (dev-only)
    printf("\fSorted pairs:\n");
    for (int i = 0; i < candidateCount; i++)
        for (int j = 0; j < candidateCount; j++)
            if (locked[i][j])
                printf("%s wins over %s \n", candidates[i], candidates[j]);

    // Get the winner
    return candidates[getSource(locked, candidateCount)];
}

// Prompt each user for their preferences and return the pointer to the votes array
int **vote(int voteCount, int candidateCount, char **candidates)
{
    // Allocate the votes array
    int **votes = (int **)malloc(voteCount * sizeof(int *));
    for (int i = 0; i < voteCount; i++)
    {
        votes[i] = (int *)malloc(candidateCount * sizeof(int));
        for (int j = 0; j < candidateCount; j++)
            /*
                 Set all values to -1 to signify that no index is chosen
                 (it's actually important, because if we check for duplicate entries, no value can be
                 interpreted as 0 in some cases, and then the program will think that the candidate
                 under index 0 was already chosen)
                */
            votes[i][j] = -1;
    }
    // Prompt each user for their preferences
    for (int i = 0; i < voteCount; i++)
    {
        printf("\fVoter %i \n", i + 1);
        // Prompt a user for each of their preference
        for (int j = 0; j < candidateCount; j++)
        {
            int index = -1;
            while (true)
            {
                // Prompt the user for a candidate
                char *name = get_string(" Position %i: ", j + 1);

                // A custom exit sequence (so I dont have to kill the console each time I
                // need to exit the program)
                if (strcmp(name, "exit") == 0)
                    exit(1);

                // Get the chosen candidate's index
                index = getIndex(name, candidateCount, candidates);

                // Check for duplicates
                bool alreadySelected = false;
                if (index != -1)
                {
                    for (int k = 0; k < candidateCount; k++)
                        if (votes[i][k] == index)
                        {
                            printf(" Candidate %s already chosen, their position: %i \n", candidates[index], k + 1);
                            alreadySelected = true;
                            break;
                        }
                }

                if (!alreadySelected)
                    if (index == -1)
                        printf(" Invalid candidate \n");
                    else
                        break;
            }
            // Assign the candiate to their position on current voter's ballot
            votes[i][j] = index;
        }
    }
    return votes;
}
// This is dev-only function that returns the randomly generated votes array
int **voteTest(int voteCount, int candidateCount, char **candidates)
{
    srand(time(NULL));
    int **votes = (int **)malloc(voteCount * sizeof(int *));
    for (int i = 0; i < voteCount; i++)
    {
        votes[i] = (int *)malloc(candidateCount * sizeof(int));
        // Initialize the ballot
        for (int j = 0; j < candidateCount; j++)
        {
            votes[i][j] = -1;
        }
        // Seed the ballot
        for (int j = 0; j < candidateCount; j++)
        {
            int key = 0;
            while (true)
            {
                key = rand() % candidateCount;
                bool isUsed = false;
                for (int k = 0; k < candidateCount; k++)
                {
                    if (votes[i][k] == key)
                    {
                        isUsed = true;
                        break;
                    }
                }
                if (!isUsed)
                {
                    break;
                }
            }
            votes[i][j] = key;
        }
    }
    return votes;
}
// Record user preferences
int **getPreferences(int voteCount, int candidateCount, int **votes)
{
    // Allocate the preferences array
    int **preferences = (int **)malloc(candidateCount * sizeof(int *));
    for (int i = 0; i < candidateCount; i++)
        preferences[i] = (int *)calloc(candidateCount, sizeof(int));

    // for each voter
    for (int i = 0; i < voteCount; i++)
    {
        // for each candidate
        for (int j = 0; j < candidateCount; j++)
        {
            // for each candidate that's lower on the list
            for (int k = candidateCount - 1; k > j; k--)
            {
                preferences[votes[i][j]][votes[i][k]]++;
            }
        }
    }
    return preferences;
}

// Return pairs array
struct pair *getPairs(int **preferences, int candidateCount)
{
    // Allocate the pairs array
    struct pair *pairs = malloc((candidateCount * (candidateCount - 1)) / 2 * sizeof(struct pair));
    // Populate the array
    for (int i = 0; i < candidateCount; i++)
        for (int j = 0; j < candidateCount; j++)
            if (j > i)
                if (preferences[i][j] - preferences[j][i] != 0)
                {
                    if (preferences[i][j] > preferences[j][i])
                        pairs[pairCount] = (struct pair){i, j, preferences[i][j]};
                    else
                        pairs[pairCount] = (struct pair){j, i, preferences[j][i]};
                    pairCount++;
                }
    // Free unused space
    pairs = realloc(pairs, pairCount * sizeof(struct pair));
    return pairs;
}

void sortPairs(struct pair *pairs)
{
    // Selection sort (O(n^2) time, O(1) space)
    for (int i = 0; i < pairCount; i++)
    {
        int biggestIndex = i;
        for (int j = i + 1; j < pairCount; j++)
        {
            if (pairs[j].winnerPreferences > pairs[biggestIndex].winnerPreferences)
            {
                biggestIndex = j;
            }
        }
        struct pair biggestPair = pairs[biggestIndex];
        pairs[biggestIndex] = pairs[i];
        pairs[i] = biggestPair;
    }
}
bool **lockPairs(struct pair *pairs, int candidateCount)
{
    // Allocate the locked array
    bool **locked = malloc(candidateCount * sizeof(bool *));
    for (int i = 0; i < candidateCount; i++)
    {
        locked[i] = calloc(candidateCount, sizeof(bool));
    }
    bool *visited = calloc(pairCount, sizeof(bool));

    // Lock the pairs
    for (int i = 0; i < pairCount; i++)
    {
        // Allocate the visited array
        bool *visited = calloc(candidateCount, sizeof(bool));
        if (!wouldCreateCycle(pairs[i].winner, pairs[i].looser, locked, candidateCount, visited))
            locked[pairs[i].winner][pairs[i].looser] = true;
    }
    return locked;
}

// Find the winner
int getSource(bool **locked, int candidateCount)
{
    for (int i = 0; i < candidateCount; i++)
    {
        // Check if the candidate has other candidates pointing at them
        bool lost = false;
        for (int j = 0; j < candidateCount; j++)
            if (locked[j][i])
                lost = true;

        // Return the index of the winner
        if (!lost)
            return i;
    }
}

// HELPER FUNCTIONS
bool wouldCreateCycle(int winner, int current, bool **locked, int candidateCount, bool *visited)
{
    if (locked[current][winner])
        return true;

    visited[current] = true;

    for (int i = 0; i < candidateCount; i++)
        if (locked[current][i] && !visited[i])
            if (wouldCreateCycle(winner, i, locked, candidateCount, visited))
                return true;

    return false;
}
// Return the index of the candidate by name (-1 if name not found)
int getIndex(char *name, char candidateCount, char **candidates)
{
    for (int i = 0; i < candidateCount; i++)
        if (strcmp(candidates[i], name) == 0)
            return i;

    return -1;
}

// Prompt the user for the number of votes and return the result
int numOfVotes(void)
{
    while (true)
    {
        int res = get_int("Number of votes: ");
        if (res < MIN_VOTES)
            printf("Minimum number of votes: %i\n", MIN_VOTES);
        else
            return res;
    }
}
