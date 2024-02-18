/**
 * Chu Hui Ong, CS50 Winter 2024
 * 
 * pagedir.c -- a module to help the crawler write pages to the pageDirectory
 *
 * This module provides functionality to initialize page directories, save webpages to files within
 * those directories, load webpages from those files, validate page directories, and retrieve URLs
 * from document files. It is primarily used by the TSE's crawler and querier modules.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <math.h>
#include "pagedir.h"
#include "webpage.h"
#include "mem.h"
#include "file.h"

/**************** global functions ****************/
bool pageDirInit(const char *pageDirectory);
void pageDirSave(webpage_t *page, const char* pageDirectory, int fn);
int pageDirLoad(webpage_t **page, const char* pageDirectory, int docID);
bool pageDirValidate(const char* pageDirectory);


/**
 * Initializes a directory for storing crawled pages. This function creates a 
 * special file named '.crawler' within the directory to mark it as a page directory.
 * 
 * @param pageDirectory The directory path to initialize.
 * @return True if the directory was successfully initialized, false otherwise.
 */
bool pageDirInit(const char *pageDirectory) {
    // Check for valid input
    if (!pageDirectory) {
        return false;
    }

    // Verify directory exists
    DIR* dir = opendir(pageDirectory);
    if (!dir) {
        return false;
    }
    closedir(dir);

    // Create '.crawler' file within the directory
    char crawlerMark[strlen(pageDirectory) + 10]; // Allocate space for the path
    sprintf(crawlerMark, "%s/.crawler", pageDirectory);
    FILE *fp = fopen(crawlerMark, "w");
    if (!fp) {
        return false;
    }
    fclose(fp);
    return true;
}

/**
 * Saves a webpage's content to a file within a page directory. The file is named using
 * an integer ID, and it contains the webpage's URL, depth, and HTML content.
 * 
 * @param page The webpage to save.
 * @param pageDirectory The directory where the page will be saved.
 * @param fn The file ID used for naming the file.
 */
void pageDirSave(webpage_t *page, const char* pageDirectory, int fn) {
    if (!page || !pageDirectory || fn < 0) {
        return; // Invalid input handling
    }

    // Construct the file name and open the file
    char fileName[strlen(pageDirectory) + (int) log10(fn) + 3];
    sprintf(fileName, "%s/%d", pageDirectory, fn);
    FILE *fp = fopen(fileName, "w");
    if (!fp) {
        return;
    }

    // Write the webpage data to the file
    fprintf(fp, "%s\n%d\n%s\n", webpage_getURL(page), webpage_getDepth(page), webpage_getHTML(page));
    fclose(fp);
}

/**
 * Loads a webpage from a file within a page directory. The file should contain the webpage's
 * URL, depth, and HTML content, in that order.
 * 
 * @param page Pointer to a webpage pointer where the loaded webpage will be stored.
 * @param pageDirectory The directory containing the webpage file.
 * @param docID The file ID corresponding to the webpage.
 * @return 1 on success, -1 if the file is invalid, and 0 for other failures.
 */
int pageDirLoad(webpage_t **page, const char* pageDirectory, int docID) {
    if (!page || !pageDirectory || docID < 1) {
        return 0; // Invalid input handling
    }

    // Construct the file name and open the file
    char docFile[strlen(pageDirectory) + (int) log10(docID) + 2];
    sprintf(docFile, "%s/%d", pageDirectory, docID);
    FILE *fp = fopen(docFile, "r");
    if (!fp) {
        return -1; // File not found
    }

    // Read the webpage data from the file
    char *url = file_readLine(fp);
    char *depthStr = file_readLine(fp);
    char *html = file_readFile(fp);
    int depth = atoi(depthStr);
    
    // Allocate and initialize the webpage
    *page = webpage_new(url, depth, html);
    if (!*page) {
        fclose(fp);
        mem_free(html);
        mem_free(url);
        mem_free(depthStr);
        return 0; // Allocation failure
    }

    // Clean up and return success
    fclose(fp);
    mem_free(depthStr);
    return 1;
}

/**
 * Validates that a given directory is a page directory initialized by the crawler.
 * This is done by checking for the presence of a '.crawler' file within the directory.
 * 
 * @param pageDirectory The directory to validate.
 * @return True if the directory is a valid page directory, false otherwise.
 */
bool pageDirValidate(const char* pageDirectory) {
    if (!pageDirectory) {
        return false;
    }

    // Check for '.crawler' file within the directory
    char crawlerMark[strlen(pageDirectory) + 11];
    sprintf(crawlerMark, "%s/.crawler", pageDirectory);
    FILE *fp = fopen(crawlerMark, "r");
    if (!fp) {
        return false; // '.crawler' file not found
    }

    fclose(fp);
    return true;
}

/**
 * Retrieves the URL of a webpage from a document file within a page directory.
 * 
 * @param pageDirectory The directory containing the webpage files.
 * @param docID The document ID corresponding to the webpage.
 * @return The URL string if found, NULL otherwise. Caller must free the returned string.
 */
char *getPageUrl(const char *pageDirectory, const int docID) {
    if (!pageDirectory || docID < 0 || !pageDirValidate(pageDirectory)) {
        return NULL;
    }

    // Construct the file name and open the file
    char docFile[strlen(pageDirectory) + (int) log10(docID) + 2];
    sprintf(docFile, "%s/%d", pageDirectory, docID);
    FILE *fp = fopen(docFile, "r");
    if (!fp) {
        return NULL;
    }

    // Read the URL from the file
    char *url = file_readLine(fp);
    fclose(fp);
    return url; // Caller is responsible for freeing the URL string
}