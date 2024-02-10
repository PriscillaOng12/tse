/**
 * Chu Hui Ong, Winter 2024, tse indexer
 * 
 * index.c - Implementation of the 'index' module for the Tiny Search Engine (TSE)
 *
 * The 'index' module provides a core functionality for the Tiny Search Engine by mapping
 * words to their occurrences within documents. This module defines and manages an 'index',
 * which is essentially a mapping from words to a set of (document ID, count) pairs, where
 * 'count' indicates how many times the word appears in the document identified by 'document ID'.
 *
 * This module is designed to be used by the TSE indexer component for indexing webpages
 * crawled by the TSE crawler, and by the TSE querier component for searching the index.
 */

#include <stdlib.h>
#include <string.h>
#include "webpage.h"
#include "hashtable.h"
#include "counters.h"
#include "file.h"
#include "mem.h"

/**************** global types ****************/
typedef struct index {
    hashtable_t* table;
} index_t;


/**************** global functions ****************/
index_t* index_new(const int size);
void index_save(index_t* index, char* indexFilename);
void index_load(index_t* index, char* indexFilename);
void index_delete(index_t* index);
void index_search(index_t* index, char** words, int* scores, int numDocs, int numWords);

counters_t* index_find(index_t* index, char* key);
bool index_add(index_t* index, char* key, int docID);
int num_docs_crawled(char* pageDirectory);

static void index_itr(void* fp, const char* key, void* item);
static void index_itr_helper(void* fp, const int key, const int count);
static void index_delete_helper(void* item);

#define DOC_ID_MAX_LEN 20 // Assuming 20 is an adequate length


/**************** local functions ****************/

/**************** index_new() ****************/
index_t* 
index_new(const int size)
{   
    if (size <= 0) {
        return NULL;
    }
    // allocate memory for array of pointers
    index_t* index = mem_malloc(sizeof(index_t));
    if (index == NULL) {
        return NULL;
    } else {
        // allocs mem
        index->table = hashtable_new(size);
    }
    
    return index;
}


/**************** index_save() ****************/
void index_save(index_t* index, char* indexFilename) {
    if (index == NULL || index->table == NULL) {
        fprintf(stderr, "ERROR: Null index or index table.\n");
        return;
    }

    FILE* fp = fopen(indexFilename, "w");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Cannot write to %s\n", indexFilename);
        return;
    }

    hashtable_iterate(index->table, fp, index_itr);
    fclose(fp);
}

/**************** index_load() ****************/
void index_load(index_t* index, char* indexFilename) {
    FILE* fp = fopen(indexFilename, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Unable to open file: %s\n", indexFilename);
        exit(3); // Consider changing this to return an error code instead of exiting.
    }

    char line[1024]; // Assuming a single line won't exceed 1024 characters.
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Trim newline character which fgets may include
        line[strcspn(line, "\n")] = 0;

        char* word = strtok(line, " ");
        if (word == NULL) {
            continue; // Skip empty lines.
        }

        // Manually duplicate the word
        char* wordCopy = malloc(strlen(word) + 1); // Allocate memory for the word copy.
        if (wordCopy == NULL) {
            // Handle allocation failure if necessary.
            continue;
        }
        strcpy(wordCopy, word); // Copy the word into the newly allocated memory.

        counters_t* counter = counters_new();
        if (counter == NULL) {
            // Handle allocation failure if necessary.
            free(wordCopy);
            continue;
        }

        char* token;
        int docID, count;
        while ((token = strtok(NULL, " ")) != NULL) {
            docID = atoi(token);
            token = strtok(NULL, " ");
            if (token == NULL) break; // In case of malformed line.
            count = atoi(token);

            counters_set(counter, docID, count); // Add docID, count pair to counter.
        }

        // Insert the word and its counter into the hashtable.
        // If the word is already present, you might want to handle it (not shown here).
        hashtable_insert(index->table, wordCopy, counter);
    }

    fclose(fp);
}


/**************** index_delete() ****************/
void index_delete(index_t* index) {
    hashtable_delete(index->table, index_delete_helper);
}

static void index_delete_helper(void* item) {
    counters_delete(item);
}


static void index_itr(void* arg, const char* key, void* item) {
    FILE* fp = (FILE*)arg; // More explicit casting for clarity

    // Print the word
    fprintf(fp, "%s", key);

    // Iterate through every docID, count pair
    counters_iterate(item, fp, index_itr_helper);

    // New line for every entry
    fprintf(fp, "\n");
}


static void index_itr_helper(void* arg, const int key, const int count) {
    FILE* fp = (FILE*)arg; // Explicit casting for clarity

    // Add [docID, count] pair to index
    fprintf(fp, " %d %d", key, count);
}


/**************** index_find() ****************/
counters_t* 
index_find(index_t* index, char* key)
{
    return hashtable_find(index->table, key);
}

/**************** index_add() ****************/
bool index_add(index_t* index, char* key, int docID) {
    if (index == NULL || key == NULL) {
        return false;
    }

    counters_t* counter = hashtable_find(index->table, key);
    if (counter == NULL) {
        counter = counters_new();
        counters_add(counter, docID);
        return hashtable_insert(index->table, key, counter);
    } else {
        return counters_add(counter, docID);
    }
}


/**************** index_search() ****************/
void index_search(index_t* index, char** words, int* scores, int numDocs, int numWords) {
    // Initialize scores array to zero
    memset(scores, 0, sizeof(int) * numDocs);

    int minScore = 0, curScore = 0;
    bool withinAND = false, firstAND = true;

    for (int i = 0; i < numWords; i++) {
        if (strcmp(words[i], "and") == 0) {
            // Switch to AND logic
            withinAND = true;
            if (!firstAND) {
                // Add the minScore from previous AND segment to all docs and reset minScore
                for (int d = 0; d < numDocs; d++) {
                    scores[d] += minScore;
                }
                minScore = 0;
            }
            firstAND = false;
        } else if (strcmp(words[i], "or") == 0) {
            // Switch to OR logic, add minScore from AND segment to all docs and reset
            withinAND = false;
            for (int d = 0; d < numDocs; d++) {
                scores[d] += minScore;
            }
            minScore = 0;
            firstAND = true; // Reset for the next AND segment
        } else {
            // Process a word
            counters_t* counter = index_find(index, words[i]);
            if (counter != NULL) {
                for (int d = 0; d < numDocs; d++) {
                    curScore = counters_get(counter, d + 1);
                    if (withinAND) {
                        if (firstAND || curScore < minScore) {
                            minScore = curScore; // Update minScore in AND logic
                        }
                    } else {
                        scores[d] += curScore; // Add directly in OR logic
                    }
                }
            }
        }
    }

    // Handle the last AND segment if the query ends with AND logic
    if (withinAND) {
        for (int d = 0; d < numDocs; d++) {
            scores[d] += minScore;
        }
    }
}


/**************** num_docs_crawled() ****************/
int num_docs_crawled(char* pageDirectory) {
    int docID = 1;
    char filePath[strlen(pageDirectory) + DOC_ID_MAX_LEN + 2]; // +1 for '/', +1 for '\0'

    while (true) {
        sprintf(filePath, "%s/%d", pageDirectory, docID);
        FILE* fp = fopen(filePath, "r");
        if (fp == NULL) break;
        fclose(fp);
        docID++;
    }

    return docID - 1;
}
