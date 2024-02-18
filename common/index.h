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

#include "hashtable.h"
#include "counters.h"

#ifndef IndexCoeff
#define IndexCoeff 825 // Default size for hashtable; can be overridden at compile time
#endif

/**
 * @file index.h
 * @brief Defines the interface for the index module in TSE.
 *
 * The index module provides an abstraction for a hashtable where each entry
 * maps a unique word (string) to a counters set. Each counters set maps document
 * IDs (integers) to the frequency (integer) of the word in that document.
 * This structure is used for efficient word lookup and frequency counting,
 * serving as an inverted index in a tiny search engine.
 */

// Type definition for index_t as a wrapper around hashtable_t for semantic clarity
typedef struct hashtable index_t;

/**
 * @brief Initializes a new index with a specified number of slots.
 *
 * @param slots The number of slots in the hashtable underlying the index.
 *              Must be a positive number.
 * @return A pointer to the newly created index, or NULL if memory allocation fails.
 */
index_t *indexInit(const int slots);

/**
 * @brief Adds a word to the index or increments its frequency for a given document.
 *
 * If the word is not already present in the index, it is added with an initial
 * frequency of 1 for the specified document. If the word exists, its frequency
 * for the given document ID is incremented.
 *
 * @param index The index to update.
 * @param word The word to add or update in the index.
 * @param docID The document ID where the word is found.
 * @return 0 on success, -1 on failure (e.g., invalid arguments or memory allocation failure).
 */
int indexAdd(index_t *index, const char *word, const int docID);

/**
 * @brief Retrieves the counters set associated with a given word in the index.
 *
 * @param index The index to search within.
 * @param word The word to look up in the index.
 * @return A pointer to the counters set associated with the word, or NULL if the word is not found.
 */
counters_t *indexFind(index_t *index, const char *word);

/**
 * @brief Updates the frequency of a word for a given document in the index.
 *
 * This function directly sets the frequency of the word for the specified document ID,
 * overwriting any previous value.
 *
 * @param index The index to update.
 * @param word The word whose frequency is to be updated.
 * @param docID The document ID for which to set the frequency.
 * @param freq The new frequency to set for the word in the specified document.
 * @return 0 on success, -1 on failure (e.g., invalid arguments or memory allocation failure).
 */
int indexUpdate(index_t *index, const char *word, const int docID, const int freq);

/**
 * @brief Loads an index from a file.
 *
 * The file is expected to have a specific format where each line contains a word
 * followed by a sequence of docID-count pairs representing the frequency of the word
 * in each document.
 *
 * @param fn The filename from which to load the index.
 * @return A pointer to the loaded index, or NULL on failure (e.g., file not found or invalid format).
 */
index_t *indexLoad(const char* fn);

/**
 * @brief Frees all memory associated with an index and deletes it.
 *
 * @param index The index to delete.
 */
void indexDelete(index_t *index);

/**
 * @brief Saves an index to a file.
 *
 * The output file will contain one line per word, with each line listing the word
 * followed by its docID-count pairs.
 *
 * @param index The index to save.
 * @param fn The filename to which the index should be saved.
 */
void indexSave(index_t *index, const char *fn);
