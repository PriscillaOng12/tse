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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "word.h"
#include "hashtable.h"
#include "counters.h"
#include "math.h"
#include "mem.h"
#include "index.h"
#include "file.h"

/* extends to hashtable struct type into an index_t type */
typedef struct hashtable index_t;

/**************** global functions ****************/
index_t *indexInit(const int slots);
int indexAdd(index_t *index, const char *word, const int docID);
counters_t *indexFind(index_t *index, const char *word);
int indexUpdate(index_t *index, const char *word, const int docID, const int freq);
index_t *indexLoad(const char* fn);
void indexDelete(index_t *index); 
void indexSave(index_t *index, const char *fn);

// Forward declarations for local helper functions
static void printIndexRow(void *fp, const char *key, void *value);
static void printCounter(void *fp, const int key, const int value);
static char *getWordInLine(const char *line, int *pos);


/* Initialize an index with a specified number of slots */
index_t *indexInit(const int slots) {
    if (slots <= 0) return NULL;
    index_t *index = (index_t *) hashtable_new(slots);
    return index;
}

/* Add or update a word's count for a specific document in the index */
int indexAdd(index_t *index, const char *word, const int docID) {
    if (index == NULL || word  == NULL || docID < 1) {  // validate arguments
        return -1;
    }
    if (normalizeWord((char *) word) != 0) {    // normalize word
        return -1;
    }
    hashtable_t *table = (hashtable_t *) index; // cast index into hashtable
    counters_t *counters = (counters_t *) hashtable_find(table, word);  // find the counters for word in index
    bool insertAfter = false;
    if (counters == NULL) {
        counters = counters_new(); // make new caounter if not found
        if (counters == NULL) {
            return -1;
        }
        insertAfter = true;     // flag to insert counter into table
    }
    if (counters_add(counters, docID) < 1) {    // add docid instance to counters
        return -1;
    }
    if (insertAfter) {
        if (!hashtable_insert(table, word, (void *) counters)) {
            return -1;
        }
    }
    return 0;
}


/* Find the counters set associated with a word in the index */
counters_t *indexFind(index_t *index, const char *word) {
    if (index == NULL || word == NULL) {    // validate args
        return NULL;
    }
    if (normalizeWord((char *) word) != 0) {    // normalize word
        return NULL;
    }
    hashtable_t *table = (hashtable_t *) index;
    counters_t *counters = (counters_t *) hashtable_find(table, word); // find counters for word
    return counters;    // return counter
}

/* Set a specific count for a word and document in the index */
int indexUpdate(index_t *index, const char *word, const int docID, const int freq) {
    if (index == NULL || word == NULL || docID < 1 || freq < 1) {   // validate args
        return -1;
    }
    bool insertAfter = false;   // track if we need to insert counter into index
    hashtable_t *table = (hashtable_t *) index;
    counters_t *counters = (counters_t *) hashtable_find(table, word);  // get counters form index
    if (counters == NULL) {
        counters = counters_new();  // make a new counter if word not in index yet
        if (counters == NULL) {
            return -1;
        }
        insertAfter = true; //  note to insert counter into index
    }
    if (!counters_set(counters, docID, freq)) { // set value of docid in counters
        return -1;
    }
    if (insertAfter) {  // insert counters into index
        if (!hashtable_insert(table, word, (void *) counters)) {
            return -1;
        }
    }
    return 0;
}

/* Load an index from a file */
index_t *indexLoad(const char* fn) {
    if (fn == NULL) {   // validate arguments
        return NULL;
    }
    index_t *index = indexInit(IndexCoeff); // get and index
    if (index == NULL) {
        return NULL;
    }
    FILE *fp = fopen(fn, "r");  // open file in read mode
    if (fp == NULL) {
    }
    char *line;
    for (line = file_readLine(fp); line != NULL; line = file_readLine(fp)) {    // read line
        int pos = 0;    // will be neeed by helper function to track position in line
        char *word = getWordInLine(line, &pos);   // get a word from line (word is any contiguous charaters not includeing space and new lines and null)
        if (word == NULL) {    // ensure word was got successfuly
            mem_free(line);
            return NULL;
        }
        for (char *token = getWordInLine(line, &pos); token != NULL; token = getWordInLine(line, &pos)) {
            if (token == NULL) {    // ensure token was got
                break;
            }
            int docID = atoi(token);    // convert to integer
            if (docID < 1) {    // ensure conversion was successful
                break;
            }
            mem_free(token);    // free token
            token = getWordInLine(line, &pos);  // get next word (freq)
            if (token == NULL) { // ensure token was got
                break;
            }
            int freq = atoi(token);     // convert to integer
            if (freq < 1) {  // ensure conversion was successful
                break;
            }
            indexUpdate(index, word, docID, freq); // update index for word -> docID with freq
            mem_free(token);
        }
        // free line and word
        mem_free(line);
        mem_free(word);
    }
    if (fp != stdin) {  // close file
        fclose(fp);
    }
    return index;
}


/* Clean up and delete an index */
void indexDelete(index_t *index) {
    if (index == NULL) {    // validate arguments
        return;
    }
    hashtable_t *table = (hashtable_t *) index; // cast to hashtable
    hashtable_delete(table, (void (*)(void *)) counters_delete);    // delete hashtable
}

/* Save an index to a file */
void indexSave(index_t *index, const char *fn) {
    if (index == NULL || fn == NULL) {  // validate arguments
        return;
    }
    FILE *fp;
    if (strncmp(fn, "-", 1) == 0) { // saved file name for stdout
        fp = stdout;
    } else {
        fp = fopen(fn, "w");    // open file in write mode
    }
    if (fp == NULL) {
        return;
    }
    hashtable_t *table = (hashtable_t *) index; // cast to hashtable
    hashtable_iterate(table, fp, printIndexRow);    // iterate through index to get values and write to file
    fclose(fp); // close file
}

/* Helper function to print a single index row */
static void printIndexRow(void *fp, const char *key, void *value) {
    if (fp == NULL || key == NULL || value == NULL) {   // validate arguments
        return;
    }
    counters_t *counters = (counters_t *) value;    // cast to couters
    fprintf(fp, "%s ", key);
    counters_iterate(counters, fp, printCounter);   // iterate counter t oget values and write to file
    fprintf(fp, "\n");
}

/* Helper function to print a counter using iterate */
static void printCounter(void *fp, const int key, const int value) {
    if (fp == NULL || key < 0 || value < 0) {   // validate  arguments
        return;
    }
    fprintf(fp, " %d %d", key, value);  // write values to file
}

/* Helper to get the next word in null terminated string line object starting from pos */
static char *getWordInLine(const char *line, int *pos) {
    if (line == NULL || pos == NULL) {  // validate arguments
        return NULL;
    }
    if (*pos >= strlen(line)) { // check if start point is in range of string
        return NULL;
    }
    int end = *pos;
    bool startedWord = false;
    for (; end < strlen(line); end++) { // loop till end of line
        if (!startedWord) { // if start of a new word has not been found in line
            if (line[end] == ' ' || line[end] == '\n') {    // if still not a new word
                (*pos)++;   // increments starting position
            } else {
                startedWord = true; // note that a new word has been found
            }
        } else {
            if (line[end] == ' ' || line[end] == '\n') {    // if word has been found and is over break
                break;
            }
        }
    }
    if (*pos >= strlen(line)) return NULL;  // is pos is out of range of the string return NULL
    
    char *token = mem_calloc((end - (*pos)) + 1, sizeof(char)); // malloc space for token/word
    if (token == NULL) {    // ensure malloc was successful 
        return NULL;
    }
    strncpy(token, line + *pos, end - *pos);    // copy word found into token
    *pos = end; // update value of pos to current word end
    return token;   // return token
}