// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// Number of buckets in hash table
const unsigned int bucketCount = 150000;

// Hash table
node **table = NULL;

unsigned int tableSize = 0;
// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // Get hash
    int pos = hash(word);

    // Get first node at that position
    node *current = table[pos];

    // Check if words match
    while (current != NULL)
    {
        // Compare two words
        bool different = false;

        for (int i = 0; i < strlen(word); i++)
            if (isalpha(word[i]) || word[i] == '\'')
            {
                if (toupper(word[i]) != toupper(current->word[i]))
                {
                    different = true;
                    break;
                }
            }
            else
            {
                different = true;
                break;
            }

        // Return true if no differences found
        if (!different)
        {
            return true;
        }
        else
        {
            // Otherwise move on to the next node
            current = current->next;
        }
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    int hash = 1;

    for (int i = 1; i < strlen(word); i++)
        hash *= toupper(word[i]);

    // Return the value that is always smaller than bucket count
    return hash % bucketCount;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // Try to open the file
    FILE *dict = fopen(dictionary, "r");

    // Check the pointer's value
    if (dict == NULL)
    {
        printf("Could not open file %s\n", dictionary);
        return false;
    }

    // Allocate a table
    table = malloc(sizeof(node) * bucketCount);

    // Allocate the size variable
    // Add words to the table
    char c;                               // initialise a char buffer
    int index = 0;                        // char position in a word
    node *current = malloc(sizeof(node)); // initialize a pointer to current node
    while (fread(&c, sizeof(char), 1, dict))
    {
        // Check if char is allowed
        if (isalpha(c) || c == '\'')
        {
            current->word[index] = c;
            index++;
        }
        else
        {
            // Null-terminate the current word
            current->word[LENGTH] = '\0';

            // Get position for the node
            int pos = hash(current->word);
            if (table[pos] != NULL)
            {
                // Append the word to the end of the linked list
                node *prev = table[pos];
                while (prev->next != NULL)
                {
                    prev = prev->next;
                }
                prev->next = current;
            }
            else
            {
                // Add current to table[pos]
                table[pos] = current;
            }

            // Increment table size
            tableSize++;

            // Prepare the index for the next node
            index = 0;
            // Assign a new pointer to current
            current = malloc(sizeof(node));
        }
    }
    // Free the word pointer
    free(current);

    if (ferror(dict))
    {
        printf("Error reading from file: %s \n", dictionary);
        fclose(dict);
        return false;
    }
    fclose(dict);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return tableSize;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    for (int i = 0; i < bucketCount; i++)
    {
        node *current = table[i]; // get the current node
        while (current != NULL)
        {
            node *next = current->next; // save the pointer to the next node
            free(current);              // free the current node
            current = next;             // move on to the next node
        }
    }
    free(table); // free the table
    return true;
}
