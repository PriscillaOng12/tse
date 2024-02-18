/**
 * Chu Hui Ong, CS50 Winter 2024
 * 
 * pagedir.h -- header file for CS50 'pagedir' module
 * 
 * This module facilitates operations on directories used by a web crawler to store webpages.
 * It includes functionality to initialize a new page directory, save webpages to the directory,
 * load webpages from the directory, validate the directory, and retrieve URLs for specific document IDs.
 */

#ifndef __PAGE_DIR_H_
#define __PAGE_DIR_H_

#include "webpage.h" 

/**
 * @brief Initializes a page directory and creates a .crawler file within it to mark it as a crawler directory.
 *
 * @param pageDirectory The path to the page directory to initialize.
 * @return True if the directory is successfully initialized, false otherwise (e.g., if the directory cannot be created or accessed).
 */
bool pageDirInit(const char *pageDirectory);

/**
 * @brief Saves a webpage to a file within the specified page directory.
 *
 * The webpage's URL, depth, and HTML content are written to a file named using the given file ID.
 *
 * @param page The webpage to save.
 * @param pageDirectory The path to the page directory where the webpage should be saved.
 * @param fn The file ID to use for naming the saved webpage file.
 *
 * Note: If any of the parameters are invalid (e.g., NULL page or pageDirectory, or negative fn), the function does nothing.
 */
void pageDirSave(webpage_t *page, const char* pageDirectory, const int fn);

/**
 * @brief Loads a webpage from a file within a crawler directory into a webpage object.
 *
 * @param page A pointer to a webpage object pointer where the loaded webpage will be stored.
 * @param pageDirectory The path to the crawler directory containing the webpage file.
 * @param docID The document ID corresponding to the webpage file to load.
 * @return -1 if the webpage file is invalid, 0 for other failures, and 1 on success.
 */
int pageDirLoad(webpage_t **page, const char* pageDirectory, int docID);

/**
 * @brief Validates that the specified directory exists and is a crawler directory.
 *
 * A directory is considered a crawler directory if it contains a .crawler file.
 *
 * @param pageDirectory The path to the directory to validate.
 * @return True if the directory exists and is a crawler directory, false otherwise.
 */
bool pageDirValidate(const char* pageDirectory);

/**
 * @brief Retrieves the URL of a webpage given its document ID within a page directory.
 *
 * @param pageDirectory The path to the page directory containing the webpage files.
 * @param docID The document ID of the webpage whose URL is to be retrieved.
 * @return A dynamically allocated string containing the URL of the webpage, or NULL on failure.
 * Note: The caller is responsible for freeing the returned string.
 */
char *getPageUrl(const char *pageDirectory, const int docID);

#endif // __PAGE_DIR_H_
