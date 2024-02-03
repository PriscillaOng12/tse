/**
 * Chu Hui Ong, CS50 Winter 2024
 * 
 * pagedir.h -- header file for CS50 'pagedir' module
 * 
*/

#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include "../libcs50/webpage.h"

/**
 * Initializes a directory for use by the crawler.
 * 
 * Attempts to mark the provided directory as being used by the crawler by creating
 * a .crawler file within it. This serves as an indication that the directory is intended
 * for crawler output.
 *
 * @param pageDirectory The directory to initialize for the crawler.
 * @return true if the directory was successfully initialized, false otherwise.
 */
bool pagedir_init(const char* pageDirectory);

/**
 * Saves a webpage to the specified directory with a document ID.
 * 
 * Writes the webpage's URL, depth, and HTML content to a file named after the document ID
 * within the specified directory. The file format is as follows:
 *  - First line: the webpage's URL
 *  - Second line: the webpage's depth
 *  - Remaining lines: the webpage's HTML content
 *
 * @param page The webpage to save.
 * @param pageDirectory The directory where the webpage file will be saved.
 * @param docID The document ID to use for the filename.
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H
