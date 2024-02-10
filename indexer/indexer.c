/**
 * Chu Hui Ong. Winter 24, tse, indexer
 * filename: indexer.c
 * usage: reads the document files produced by the TSE crawler, builds an index, and writes that index to a file.
*/


#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "index.h"
#include "webpage.h"
#include "file.h"
#include "mem.h"

// internal function prototypes
static void indexBuild(index_t* index, char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, int docID);

/**************** main ****************/
/**
 * parse the command line, validate parameters, initialize other modules
 * call indexBuild, with pageDirectory
*/
int main(int argc, char *argv[])
{
    /* parse the command line, validate parameters */ 
    // check num parameters
    if (argc != 3) {
        fprintf(stderr, "ERROR: Expected 2 arguments but recieved %d\n", argc-1);
        exit(1);
    }
    // Defensive programming
    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr, "ERROR: NULL argument passed\n");
        exit(2);
    }


    /* initialize other modules */

    index_t* index = NULL;

    /* call indexBuild, with pageDirectory */
    char* pageDirectory = argv[1];
    char* indexFilename = argv[2];
    /* creates a new 'index' object */ 
    index = index_new(200);
    indexBuild(index, pageDirectory);

    /* create a file indexFilename and write the index to that file */
    index_save(index, indexFilename);
    index_delete(index);
    mem_free(index);


    return 0; // exit status
}



/**************** indexBuild() ****************/
/**
 * creates a new 'index' object
 * loops over document ID numbers, counting from 1
 * loads a webpage from the document file 'pageDirectory/id'
 * if successful, 
 * passes the webpage and docID to indexPage
*/
static void
indexBuild(index_t* index, char* pageDirectory)
{
    int docID = 1;
    char filePath[strlen(pageDirectory) + 22]; // Including space for '/' and ".crawler" and null terminator
    FILE* fp;
    webpage_t* page;

    // Construct the base file path for documents and crawler file check
    sprintf(filePath, "%s/.crawler", pageDirectory);

    // Check for crawler directory marker file
    if ((fp = fopen(filePath, "r")) == NULL) {
        fprintf(stderr, "ERROR: Crawler directory marker %s not found!\n", filePath);
        exit(4);
    }
    fclose(fp);

    // Loop over document ID numbers, starting from 1
    for (docID = 1; ; docID++) {
        sprintf(filePath, "%s/%d", pageDirectory, docID); // Construct file path for current docID

        if ((fp = fopen(filePath, "r")) == NULL) break; // Exit loop if file doesn't exist

        // Initialize webpage fields
        char* url = file_readLine(fp);
        if (url == NULL) {
            fclose(fp);
            exit(1);
        }

        int depth;
        fscanf(fp, "%d", &depth); // Read depth directly

        // Read HTML content
        fseek(fp, 0, SEEK_END); // Move to the end of the file
        long numChars = ftell(fp) - ftell(stdin) - strlen(url) - 1; // Calculate the size needed for HTML
        fseek(fp, ftell(stdin) + strlen(url) + 1, SEEK_SET); // Reset file pointer to the start of HTML content

        char* html = mem_malloc(numChars + 1); // Allocate memory for HTML content
        fread(html, 1, numChars, fp); // Read HTML content directly into allocated memory
        html[numChars] = '\0'; // Null-terminate the HTML string

        // Generate webpage from URL, depth, and HTML, then index it
        page = webpage_new(url, depth, html);
        indexPage(index, page, docID);

        // Clean up
        webpage_delete(page);
        fclose(fp);
    }
}


/**************** indexPage() ****************/
/**
 * steps through each word of the webpage,
 * skips trivial words (less than length 3),
 * normalizes the word (converts to lower case),
 * looks up the word in the index,
 * adding the word to the index if needed
 * increments the count of occurrences of this word in this docID
*/
static void
indexPage(index_t* index, webpage_t* page, int docID)
{
    int pos = 0;
    char* word;

    // Steps through each word of the webpage
    while ((word = webpage_getNextWord(page, &pos)) != NULL) {
        // Skips trivial words 
        if (strlen(word) >= 3) {
            // Normalize the word 
            for (char* ptr = word; *ptr; ptr++) {
                *ptr = tolower(*ptr);
            }

            // Looks up the word in the index and increments the word count for docID
            index_add(index, word, docID);
        }

        // Free the word regardless of whether it was added to the index
        mem_free(word);
    }
}

