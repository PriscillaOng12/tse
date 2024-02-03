/**
 * Chu Hui Ong, CS50 Winter 2024
 * 
 * pagedir.c -- a module to help the crawler write pages to the pageDirectory

*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "../libcs50/webpage.h"


/**
 * Initialize the page directory by creating a .crawler file.
 * @param pageDirectory The directory where the .crawler file will be created.
 * @return true if successful, false otherwise.
 */
bool pagedir_init(const char* pageDirectory) {
    // Construct the pathname for the .crawler file
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/.crawler", pageDirectory);

    // Open the file for writing
    FILE* fp = fopen(filepath, "w");
    if (fp == NULL) {
        // Error handling: Print an error message if the file cannot be opened
        fprintf(stderr, "Error: Could not open file %s for writing\n", filepath);
        return false;
    }

    fclose(fp);
    return true;
}


/**
 * Save a webpage to the specified page directory with a given document ID.
 * @param page The webpage to save.
 * @param pageDirectory The directory where the webpage file will be saved.
 * @param docID The document ID to use as the filename.
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, int id) {
    // Ensure the input parameters are valid
    if (page == NULL || pageDirectory == NULL) {
        fprintf(stderr, "Error: Invalid input to pagedir_save.\n");
        return; // Early return to handle invalid inputs
    }

    // Construct the filename
    char filepath[256]; 
    snprintf(filepath, sizeof(filepath), "%s/%d", pageDirectory, id);

    // Attempt to open the file for writing
    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file %s for writing.\n", filepath);
        return; // Exit the function early if unable to open the file
    }

    // Write the webpage's URL, depth, and HTML content to the file
    fprintf(fp, "%s\n%d\n%s", webpage_getURL(page), webpage_getDepth(page), webpage_getHTML(page));

    // Close the file after writing
    fclose(fp);
}
