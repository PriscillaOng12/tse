/**
 * Chu Hui Ong, Winter 2024, tse indexer
 *  
 * index.h - header file for the 'index' module
 *
 * The 'index' module defines a data structure that maps words to (document ID, count) pairs,
 * where each word is associated with multiple document IDs and each document ID has a count 
 * of how many times the word appears in that document. This module provides functionality 
 * to create, manipulate, save, load, and delete an index, as well as to perform searches 
 * within it.
 *
 */

#include <stdlib.h>
#include "hashtable.h"
#include "counters.h"
#include "mem.h"

/**************** global types ****************/
typedef struct index index_t; // Opaque type hiding the implementation of index structure.

/**************** index_new ****************/
/*
 * Initializes a new index structure.
 *
 * Inputs:
 *   size: the number of slots in the hashtable (must be greater than zero).
 *
 * Returns:
 *   A pointer to a newly allocated index structure, or NULL if there's an error in creation.
 *
 * Note:
 *   The caller is responsible for freeing this memory by calling index_delete().
 */
index_t* index_new(const int size);

/**************** index_save ****************/
/*
 * Writes the contents of the index to a file.
 *
 * Inputs:
 *   index: a pointer to the index structure to be saved.
 *   indexFilename: the pathname of the file where the index should be saved.
 *
 * Note:
 *   If the file does not exist, it will be created. If it does exist, it will be overwritten.
 *   The index structure is not modified by this operation.
 */
void index_save(index_t* index, char* indexFilename);


/**************** index_load ****************/
/*
 * Loads the contents of a file into an index structure.
 *
 * Inputs:
 *   index: a pointer to an initially empty index structure where the file contents will be loaded.
 *   indexFilename: the pathname of the file from which to load the index.
 *
 * Note:
 *   The file should be in the format produced by index_save().
 *   If the file cannot be opened or read, the function will print an error message and return.
 */
void index_load(index_t* index, char* indexFilename);


/**************** index_delete ****************/
/*
 * Frees all memory associated with the index.
 *
 * Inputs:
 *   index: a pointer to the index structure to be deleted.
 *
 * Note:
 *   After this operation, the pointer to the index should not be used again.
 */
void index_delete(index_t* index);


/**************** index_find ****************/
/*
 * Finds and returns the counters set associated with a given word.
 *
 * Inputs:
 *   index: a pointer to the index structure.
 *   key: the word to search for in the index.
 *
 * Returns:
 *   A pointer to the counters set associated with the word, or NULL if the word is not found.
 *
 * Note:
 *   The index structure is not modified by this operation.
 */
counters_t* index_find(index_t* index, char* key);


/**************** index_add ****************/
/*
 * Adds a document ID to the index with a given word, or increments the count if it already exists.
 *
 * Inputs:
 *   index: a pointer to the index structure.
 *   key: the word to be added or updated in the index.
 *   docID: the document ID where the word was found.
 *
 * Returns:
 *   True if the operation was successful, or false otherwise.
 *
 * Note:
 *   This function adds the word to the index if it does not exist, or increments the count associated
 *   with the word and document ID if it does.
 */
bool index_add(index_t* index, char* key, int docID);


/**************** index_search ****************/
/*
 * Searches the index for multiple words and calculates a relevance score for each document.
 *
 * Inputs:
 *   index: a pointer to the index structure.
 *   words: an array of words to search for.
 *   scores: an array of integers to store the relevance scores for each document.
 *   numDocs: the number of documents in the corpus.
 *   numWords: the number of words in the words array.
 *
 * Note:
 *   The function updates the scores array with the relevance scores for each document based on the search words.
 */
void index_search(index_t* index, char** words, int* scores, int numDocs, int numWords);


/**************** num_docs_crawled ****************/
/*
 * Determines the number of documents that have been crawled and stored in a directory.
 *
 * Inputs:
 *   pageDirectory: the path to the directory containing crawled documents.
 *
 * Returns:
 *   The number of documents in the directory, assuming the directory follows the crawler format.
 *   Returns 0 if the directory does not follow the expected format.
 *
 * Note:
 *   This function assumes the directory exists and has been populated by the crawler.
 */
int num_docs_crawled(char* pageDirectory);
