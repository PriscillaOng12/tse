/**
 * Chu Hui Ong. Winter 24, tse, indexer
 * filename: indextest.c
 * usage: testing module for indexer
 * loads an index file produced by the indexer and saves it to another file.
 * these two files should be equivalent after running testing script to sort
 * 
 * Exit codes: 1 -> invalid number of arguments
 *             2 -> one or more arguments are null
 *             3 -> file cannot be opened
 */

#include <stdio.h>
#include "index.c"
#include "file.h"

// internal function prototypes

/**************************** main ***************************/
/**  executed with syntax: ./indextest oldIndexFilename newIndexFilename
 *  testing module for indexer, recreates inverted index file from
 *  oldIndexFilename and writes it to newIndexFilename.
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

    // assign input to named variables
    char* oldIndexFilename = argv[1];
    char* newIndexFilename = argv[2];

    // initiate index from num of lines in file
    index_t* index = NULL;
    FILE* fp;
    if ((fp = fopen(oldIndexFilename, "r")) == NULL) {
        fprintf(stderr, "ERROR: Cannot open file %s\n", oldIndexFilename);
        fclose(fp);
        exit(3);
    }
    // if succesful, determine length of file
    const int size = file_numLines(fp);
    fclose(fp);
    // initialize index to length of file (same as num of words)
    index = index_new(size);
    
    /* load index from oldIndexFilename*/
    index_load(index, oldIndexFilename);

    /* writes index to newIndexFilename */
    index_save(index, newIndexFilename);
    index_delete(index);
    mem_free(index);

    return 0; // exit status
}