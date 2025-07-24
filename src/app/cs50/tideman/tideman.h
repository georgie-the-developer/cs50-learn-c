// tideman.h
#ifndef TIDEMAN_H
#define TIDEMAN_H

struct pair
{
    int winner;
    int looser;
    int winnerPreferences;
};

// Main functions
char *runElection(int candidateCount, char **candidates);
int **vote(int voteCount, int candidateCount, char **candidates);
int **voteTest(int voteCount, int candidateCount, char **candidates);
int **getPreferences(int voteCount, int candidateCount, int **votes);
struct pair *getPairs(int **preferences, int candidatesCount);
void sortPairs(struct pair *pairs);
bool **lockPairs(struct pair *pairs, int candidatesCount);
int getSource(bool **locked, int candidateCount);

// Helper functions
bool wouldCreateCycle(int winner, int current, bool **locked, int candidatesCount, bool *visited);
int getIndex(char *name, char candidateCount, char **candidates);
int numOfVotes(void);

#endif