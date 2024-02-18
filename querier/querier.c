/**
 * Chu Hui Ong, CS50 Winter 2024
 * 
 * querier.c -- implements the querier functionality for the Tiny Search Engine.
 * The querier reads the index produced by the TSE indexer and serves queries entered by the user,
 * matching documents in a given page directory. It supports logical operators 'AND' and 'OR' to refine searches.
 * Results are displayed in descending order of relevance, based on the number of occurrences of query terms.
 *
 * Usage: ./querier <pageDirectory> <indexFilename>
 * - <pageDirectory> is the path to the directory produced by the TSE crawler.
 * - <indexFilename> is the path to the file produced by the TSE indexer.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include "pagedir.h"
#include "index.h"
#include "bag.h"
#include "counters.h"
#include "set.h"
#include "pagedir.h"
#include "file.h"
#include "word.h"


// Node structure for a linked list to sort integer pairs.
typedef struct lnode {
    int values[2];        // Stores a pair of integers.
    struct lnode *next;   // Pointer to the next node in the list.
} lnode_t;

// Structure for passing arguments to iteration functions.
typedef struct IterArg {
    counters_t *res;      // Pointer to a structure where results are accumulated.
    counters_t *other;    // Pointer to an auxiliary structure for additional data.
    lnode_t *argNode;     // Pointer to a linked list node used for sorting purposes.
    int count;            // Tracks the number of nodes created for sorting.
} iter_arg_t;

// Prototype declaration for 'fileno' to get the file descriptor from a file stream.
int fileno(FILE *stream);

// internal function prototypes
static int parseArgs(char *args[], char **pageDir, char **indexFile);
static int query(index_t *index, char *pageDir, char **queryList);
static void readParse(index_t *index, char *pageDir);
static char *prompt();
static int tokenize(char **list, char *line);
static bool isOP(char *word);
static bool validateQuery(char **query, int querySize);
static void countersIntersect(void *arg, const int key, const int val1);
static void countersUnion(void *arg, const int key, const int val1);
static int sortPrint(counters_t *scores, char *pageDir);
static void sortIterate(void *arg, const int key, const int val);
static void copyIter(void *arg, const int key, const int val);
static void logMessage(const int argc, ...);



/**
 * @brief Parses command line arguments for the querier.
 * 
 * Validates and extracts the page directory and index file paths from command line arguments.
 * Ensures the page directory does not end with a '/' and verifies the index file can be opened.
 * Logs messages on failure for debugging purposes.
 * 
 * @param args Array of command line arguments.
 * @param pageDir Pointer to store the allocated page directory path.
 * @param indexFile Pointer to store the allocated index file path.
 * @return int Status code: 0 for success, -1 for failure.
 */
static int parseArgs(char *args[], char **pageDir, char **indexFile) {
    // Validate input pointers.
    if (args == NULL || pageDir == NULL || indexFile == NULL) {
        logMessage(3, "parseArgs: Invalid input arguments\n");
        return -1;
    }

    // Allocate and copy the page directory path, ensuring no trailing slash.
    *pageDir = calloc(strlen(args[1]) + 1, sizeof(char));
    if (*pageDir == NULL) {
        logMessage(3, "parseArgs: Failed to allocate memory for pageDir '%s'\n", args[1]);
        return -1;
    }
    strncpy(*pageDir, args[1], strlen(args[1]));
    // Remove trailing slash, if present.
    size_t dirLen = strlen(*pageDir);
    if ((*pageDir)[dirLen - 1] == '/') {
        (*pageDir)[dirLen - 1] = '\0';
    }

    // Validate the page directory.
    if (!pageDirValidate(*pageDir)) {
        logMessage(3, "parseArgs: Invalid page directory '%s'\n", *pageDir);
        free(*pageDir); // Clean up allocated memory on failure.
        *pageDir = NULL; // Set pointer to NULL to avoid accidental reuse.
        return -1;
    }

    // Allocate and copy the index file path.
    *indexFile = calloc(strlen(args[2]) + 1, sizeof(char));
    if (*indexFile == NULL) {
        logMessage(3, "parseArgs: Failed to allocate memory for indexFile '%s'\n", args[2]);
        free(*pageDir); // Clean up previously allocated memory.
        *pageDir = NULL; // Set pointer to NULL to avoid accidental reuse.
        return -1;
    }
    strncpy(*indexFile, args[2], strlen(args[2]));

    // Check if the index file can be opened.
    FILE *fp = fopen(*indexFile, "r");
    if (fp == NULL) {
        logMessage(3, "parseArgs: Failed to open indexFile '%s'\n", *indexFile);
        free(*pageDir);   // Clean up previously allocated memory.
        free(*indexFile); // Clean up allocated memory for indexFile.
        *pageDir = NULL;    // Set pointers to NULL to avoid accidental reuse.
        *indexFile = NULL;
        return -1;
    }
    fclose(fp); // Close the file as it's no longer needed.

    return 0; // Return 0 on success.
}




/**
 * @brief helper function that accepts and indexer and queries the indexer
 * 
 * @param index indexer to query
 * @param pageDir pointer to char pointer to store the page directory
 *  * @param queryList list of words in query
 * @return int return status code
 * - 0 for failure
 * - -1 for success
 */
static int query(index_t *index, char *pageDir, char **queryList) {
    if (index == NULL || pageDir == NULL || queryList == NULL) {
        logMessage(1, "query: Invalid arguments\n");
        return -1;
    }
    int return_code = 0;
    int querySize = 0;
    counters_t *prev = NULL;
    counters_t *queryResult = NULL;
    bag_t *bag = NULL;

    for(;queryList[querySize] != NULL; querySize++);
    if (querySize == 0)  {
        return_code = 0;
        goto prep_return;
    }
    if (validateQuery(queryList, querySize)) {  // validate query
        logMessage(1, "%s", "\nQuery is Valid!\n");
    } else {
        logMessage(1, "%s", "\nQuery is invalid!\n");
        return_code = -1;
        goto prep_return;
    }
    
    
    bag = bag_new();    // will keep discrete blocks ( x1 or .... or xn) of the query
    // discreet block are unionized and not intersected
    if (bag == NULL) {  // ensure bag was created
        return_code = -1;
        goto prep_return;
    }
    for (int i = 0; i < querySize;) {
        char *word1 = NULL;
        char*op = NULL;
        char *word2 = NULL;
        counters_t *counters1 = NULL;
        counters_t *counters2 = NULL;
        if (i == 0) {   // if this is the first word in the query
            word1 = queryList[i];
            if (i < querySize - 1 && isOP(queryList[i+1])) {    // if there is at least two more words in the query and next word is an operation
                op = queryList[i+1];
                word2 = queryList[i+2];
                i += 3;
            } else if(i < querySize - 1) {   // if there is at least two more words in the query and next word is not an operation
                op = "and";
                word2 = queryList[i+1];
                i += 2;
            } else {    // if there is one word in the query
                op = "or";
                i++;
            }
        } else {    // if not the first word
            if (isOP(queryList[i])) {   // is the word is an operation
                op = queryList[i];
                word1 = queryList[i + 1];
                i += 2;
            } else {    // if word is not an operation
                op = "and";
                word1 = queryList[i];
                i += 1;
            }
        }
        if (prev != NULL) { // prev is the other value to operate with
            iter_arg_t arg;
            counters1 = prev;
            counters2 = counters_new();
            arg.res = counters2;
            counters_iterate(indexFind(index, word1), (void *) &arg, copyIter); // copy index counter of word 1 to the counters1
        } else if (word2 == NULL) { // only one word in query
            iter_arg_t arg;
            counters1 = counters_new();
            arg.res = counters1;
            counters_iterate(indexFind(index, word1), (void *) &arg, copyIter); // copy index counter of word 1 to the counters1
        } else {    // first two words in query
            iter_arg_t arg;
            counters1 = counters_new();
            counters2 = counters_new();
            arg.res = counters1;
            counters_iterate(indexFind(index, word1), (void *) &arg, copyIter); // copy index counter of word 1 to the counters1
            arg.res = counters2;
            counters_iterate(indexFind(index, word2), (void *) &arg, copyIter); // copy index counter of word 2 to the counters2
        }
        if (strncmp(op, "or", 2) == 0) {    // when or use prev as discreet bock and add it to bag; prev now becomes counters2
            bag_insert(bag, (void *) counters1);
            prev = counters2;
        } else if (strncmp(op, "and", 2) == 0) {    // when and intersect counters 1 and 2
            iter_arg_t arg;
            arg.res = counters1;
            arg.other = counters2;
            counters_iterate(counters1, (void *) &arg, countersIntersect);
            if (prev == NULL) prev = counters1;
            counters_delete(counters2); // delete counters 2 (it is a copy so this does not affect the indexer )
        }
    }

    if (prev != NULL) bag_insert(bag, (void *) prev);   // put last item into the bag

    queryResult = counters_new();   // result counters
    counters_t *next;
    while((next = (counters_t *) bag_extract(bag)) != NULL) {   // extract all items from bag and unionize into result
        iter_arg_t arg;
        arg.res = queryResult;
        counters_iterate(next, (void *) &arg, countersUnion);
        counters_delete(next);
    }

    sortPrint(queryResult, pageDir);    // sort and print result

    prep_return:    // return prep location that can be jumped to from anywhere in the fucntion
        if (queryList != NULL) {
            for (int i = 0; i < querySize; i++) {
                free(queryList[i]);
            }
            free(queryList);
        }
        if (queryResult != NULL) counters_delete(queryResult);
        if(bag != NULL) bag_delete(bag, (void (*)(void *)) counters_delete);
    return return_code;
}


/**
 * @brief helper function to reads from stdin, validates input and parses into a normalized query
 * 
 * @param index indexer to query
 * @param pageDir pointer to char pointer to store the page directory
 */
static void readParse(index_t *index, char *pageDir) {
    if (index == NULL || pageDir == NULL) {
        logMessage(1, "readParse: invalid arguments\n");
        return;
    }
    char **list = NULL;
    char *line = NULL;
    
    while( ( line = prompt() ) != NULL ) {  // prompt and check line
        for (int i = 0; i < strlen(line); i++) {    // validate the characters in  the line read
            if ( !isspace(line[i]) && !isalpha(line[i]) ) {
                printf("Invalid query\n");
                logMessage(3, "%s", "\nreadParse: query (", "%s", line, "%s", ") is an invalid query!\n");
                if (line != NULL) free(line);
                line = NULL;
                break;
            }
        }
        if (line == NULL) continue;
        
        int wordAppr = (int) (strlen(line) / 2) + 1; // number of words in line is at most #chars/2 + 1
        list = calloc( wordAppr, sizeof(char *) );
        if (list == NULL) { // ensure list is not null
            logMessage(3, "%s", "\nreadParse: failed to allocate ", "%d", wordAppr * sizeof(char *), "%s", "bytes for list\n");
            if (line != NULL) free(line);
            line = NULL;
            continue;
        }
        for (int i = 0; i < wordAppr; i++) {    // preset positions in list to null
            list[i] = NULL;
        }
        if (tokenize(list, line) == -1) {   // tokenize line and save words into list
            logMessage(2, "%s", "\nreadParse: failed to tokenize line :\n", "%s", line);
            if (line != NULL) free(line);
            line = NULL;
            if (list != NULL) free(list);
            line = NULL;
            continue;
        }
        printf("Normalized Query: ");
        for (int i = 0; i < wordAppr; i++) {
            if (list[i] != NULL) printf("%s ", list[i]);
        }
        printf("\n");
        query(index, pageDir, list);    // run words in list as query
        if (line != NULL) free(line);
        line = NULL;
    }
    printf("\n");

    if (line != NULL) free(line);
    line = NULL;
    if (list == NULL) free(list);
    list = NULL;
}


/**
 * @brief helper function to prompt and read line
 * 
 * @return char* line read
 */
static char *prompt() {
    if (isatty(fileno(stdin))) { 
        printf("Query? ");
    }
    return file_readLine(stdin);
}

/**
 * @brief Tokenizes a string by spaces without using strtok or strdup, and inserts the tokens into a list.
 * 
 * Each token is normalized and stored in a dynamically allocated array of strings (`list`).
 * The function assumes that `list` has enough space for all tokens.
 * 
 * @param list Pointer to an array of character pointers to store tokens.
 * @param line The string to be tokenized.
 * @return int Returns 0 on success, -1 on failure (e.g., memory allocation failure).
 */
static int tokenize(char **list, char *line) {
    if (list == NULL || line == NULL) {
        logMessage(1, "tokenize: Invalid arguments\n");
        return -1;
    }

    int wordStart = -1; // Start index of a word; -1 indicates not currently reading a word.
    int count = 0;      // Number of words inserted into the list.
    int lineLen = strlen(line); // Length of the input line.

    for (int i = 0; i <= lineLen; i++) { // Iterate through the line including the null terminator.
        if (isspace(line[i]) || line[i] == '\0') { // Check for word boundaries (space or null terminator).
            if (wordStart >= 0) { // We have a word to process.
                int wordLen = i - wordStart;
                char *word = calloc(wordLen + 1, sizeof(char)); // +1 for null terminator.
                if (word == NULL) {
                    logMessage(1, "tokenize: Memory allocation failed for word\n");
                    return -1; // Memory allocation failed.
                }

                // Copy the word from the line.
                strncpy(word, line + wordStart, wordLen);
                word[wordLen] = '\0'; // Null-terminate the string.
                normalizeWord(word);  
                list[count++] = word; // Store the word in the list.

                wordStart = -1; // Reset wordStart to indicate we're not currently reading a word.
            }
        } else if (wordStart < 0) { // Not currently reading a word, and found a non-space character.
            wordStart = i; // Start of a new word.
        }
    }

    return 0; // Success.
}


/**
 * @brief Helper function to check if a word is an operator ("and" or "or").
 * 
 * @param word The word to check.
 * @return true if the word is exactly "and" or "or", case-insensitive.
 * @return false for any other word or NULL input.
 */
static bool isOP(char *word) {
    if (word == NULL) { // validate arguments
        logMessage(1, "Invalid arguments\n");
        return false;
    }
    return strncmp("and", word, 3) == 0 || strncmp("or", word, 2) == 0;
}


/**
 * @brief Checks if a query is valid according to specific rules.
 * 
 * A valid query cannot start or end with an operator ("and" or "or"),
 * and cannot have two operators adjacent to each other.
 * 
 * @param query Array of strings representing the query terms.
 * @param querySize The number of elements in the query array.
 * @return true if the query is valid, false otherwise.
 */
static bool validateQuery(char **query, int querySize) {
    // Validate arguments
    if (query == NULL || querySize <= 0) {
        logMessage(1, "validateQuery: Invalid arguments\n");
        return false;
    }

    // Check if the first or last word is an operator
    if (isOP(query[0])) {
        printf("'%s' cannot be first\n", query[0]);
        return false;
    }
    if (isOP(query[querySize - 1])) {
        printf("'%s' cannot be last\n", query[querySize - 1]);
        return false;
    }

    // Ensure no two operators are adjacent
    for (int i = 1; i < querySize; i++) { // Start from the second element
        if (isOP(query[i]) && isOP(query[i - 1])) {
            printf("'%s' and '%s' cannot be adjacent\n", query[i - 1], query[i]);
            return false;
        }
    }

    // If all checks pass, the query is valid
    return true;
}


/**
 * @brief Intersects two counters.
 * 
 * Iterates through one set of counters and finds the minimum value for each key
 * between the two counters. The result is stored in a third counter, effectively
 * performing the intersection operation where only the keys present in both counters
 * are retained with their minimum value.
 * 
 * @param arg A void pointer to an iter_arg_t struct containing the counters and additional arguments.
 * @param key The current key being processed in the iteration.
 * @param val1 The value associated with the key in the first counter being iterated over.
 */
static void countersIntersect(void *arg, const int key, const int val1) {
    if (arg == NULL || key < 0 || val1 < 0) {
        logMessage(1, "countersIntersect: Invalid arguments\n"); // Log error if arguments are invalid
        return; // Early return to avoid processing invalid data
    }
    iter_arg_t *args = (iter_arg_t *)arg; // Cast void pointer to iter_arg_t struct pointer for easier access

    // Get the value for the same key in the other counter
    int val2 = counters_get(args->other, key);

    // Determine the minimum value between the two counters for this key
    int min_val = (val2 < val1) ? val2 : val1;

    // Set the key's value in the result counter to the minimum value found
    counters_set(args->res, key, min_val);
}


/**
 * @brief Unions two counters.
 * 
 * Iterates through one set of counters and adds the value for each key to the value
 * of the same key in a second counter. The result is stored in the second counter,
 * effectively performing the union operation where the keys are retained with their
 * combined values.
 * 
 * @param arg A void pointer to an iter_arg_t struct containing the counters and additional arguments.
 * @param key The current key being processed in the iteration.
 * @param val1 The value associated with the key in the first counter being iterated over.
 */
static void countersUnion(void *arg, const int key, const int val1) {
    if (arg == 0 || key < 0 || val1 < 0) return;    // validate arguments
    iter_arg_t *args = (iter_arg_t *) arg;  // cat to argument struct
    int val2 = counters_get(args->res, key);    // get value in other counters
    int res = val1 + val2;  // union is the sum of value in both counters
    if (res != 0) { // only set non-zero values to reduce redundancy
        counters_set(args->res, key, res);
    }
}


/**
 * @brief Sorts the values in a counter and prints them with associated document information.
 * 
 * Iterates through a counters structure to sort its entries, then prints each entry's score
 * and associated document URL, retrieved based on the document ID stored in the counter.
 * Assumes the presence of a getPageUrl function that retrieves a document URL given its ID.
 * 
 * @param scores Counters structure containing document scores.
 * @param pageDir String containing the path to the page directory.
 * @return int Returns 0 on success, -1 on failure (invalid arguments or sorting/printing issues).
 */
static int sortPrint(counters_t *scores, char *pageDir) {
    // Validate function arguments
    if (scores == NULL || pageDir == NULL) {
        logMessage(1, "sortPrint: Invalid arguments\n");
        return -1;
    }

    iter_arg_t args;
    args.argNode = NULL; // Initialize linked list for sorted nodes
    args.count = 0;      // Initialize document count

    // Iterate through scores counters and sort them
    counters_iterate(scores, (void *)&args, sortIterate);

    // Print the number of matches. This will print "Matches 0 documents (ranked):" if args.count is 0.
    printf("Matches %d documents (ranked):\n", args.count);

    // If there are sorted items, print each item's score and associated document URL.
    for (lnode_t *node = args.argNode; node != NULL;) {
        char *url = getPageUrl(pageDir, node->values[0]); // Retrieve URL for the document ID
        if (url != NULL) {
            printf("score\t%d doc %d: %s\n", node->values[1], node->values[0], url);
            free(url); // Free the URL string allocated by getPageUrl
        } else {
            logMessage(1, "sortPrint: Failed to retrieve URL for doc %d\n", node->values[0]);
        }

        // Move to the next node and free the current node
        lnode_t *tmp = node;
        node = node->next;
        free(tmp);
    }

    return 0; // Success
}


/**
 * @brief Iterates over counters and inserts each key-value pair into a sorted linked list.
 * 
 * This function is intended to be used with counters_iterate to sort counters by their values.
 * It maintains a sorted linked list in descending order of values (counters).
 * 
 * @param arg Pointer to iter_arg_t structure containing the linked list and a count of elements.
 * @param key The key from the counters, representing, for example, a document ID.
 * @param val The value associated with the key, used for sorting.
 */
static void sortIterate(void *arg, const int key, const int val) {
    if (arg == NULL || key < 0 || val < 0) return;
    iter_arg_t *args = (iter_arg_t *) arg;  // cast to argument struct
    args->count++;  // increment count
    lnode_t *node;
    lnode_t *next = (lnode_t *) calloc(1, sizeof(lnode_t)); // make space for new node
    next->next = NULL;
    next->values[0] = key;
    next->values[1] = val;
    if (args->argNode == NULL) {    // if this is the first node
        args->argNode = next;
        return;
    }
    node = args->argNode;

    if (node->values[1] <= next->values[1]) {   // if new node should be the first node
        args->argNode = next;
        next->next = node;
    } else {
        do {    // loop thoguh nodes and insert node as appropriate
            if (node->next == NULL || node->next->values[1] <= next->values[1]) {
                next->next = node->next;
                node->next = next;
                break;  // loop should only ever escape at this point
            }
            node = node->next;
        } while( node != NULL ); // should never resolve this condition
    }
}

/**
 * @brief Copies each key-value pair from one counter to another during iteration.
 * 
 * This helper function is intended for use with counters_iterate to copy all key-value
 * pairs from the source counter to the destination counter specified in the iter_arg_t
 * structure. Each key's value from the iterating counter is set in the destination counter.
 * 
 * @param arg Pointer to iter_arg_t structure containing the destination counter (res field).
 * @param key The key from the iterating counter.
 * @param val The value associated with the key in the iterating counter.
 */
static void copyIter(void *arg, const int key, const int val) {
    if (arg == NULL || key < 0 || val < 0) {
        // Early return if arguments are invalid or if key/value are out of expected range
        return;
    }

    iter_arg_t *args = (iter_arg_t *)arg;  // Cast void pointer to iter_arg_t structure
    counters_t *destinationCounter = args->res; // Retrieve the destination counter from the argument structure

    // Set the value for this key in the destination counter to match the source counter
    counters_set(destinationCounter, key, val);
}


/**
 * @brief functinn to print pessages only when in DEV or TEST modes
 * 
 * @param argc arguments to parse and log
 * @param ... 
 */
static void logMessage(const int argc, ...) {
    #ifdef TEST
        if (argc <= 0) {
            fprintf(stderr, "\n");
            return;
        }
        va_list argv;
        va_start(argv, argc);
        int argi;
        
        for (argi = 1; argi <= argc; argi ++) {
            char *argt = va_arg(argv, char *);
            if (strncmp(argt, "%lu", strlen(argt)) == 0) {
                fprintf(stderr, "%lu", va_arg(argv, unsigned long));
            } else if(strncmp(argt, "%s", strlen(argt)) == 0) {
                fprintf(stderr, "%s",  va_arg(argv, char *));
            } else if(strncmp(argt, "%d", strlen(argt)) == 0) {
                fprintf(stderr, "%d", va_arg(argv, int));
            } else if(strncmp(argt, "%c", strlen(argt)) == 0) {
                fprintf(stderr, "%c", (char) va_arg(argv, int));
            }
        }
    #endif
}


int main(int argc, char const *argv[]) {

    if (argc != 3) {    // ensure arguments are the require number
        printf("Usage: ./querier <pageDirectory> <indexFilename>\n");
        exit(-1);
    }
    int exit_code = 0;
    char *pageDir = NULL;
    char *indexFile = NULL;
    index_t *index = NULL;

    if (parseArgs((char **) argv, &pageDir, &indexFile) == -1) {    // parse arguments into varaibles and validate them
        logMessage(5, "%s", "main: invalid arguments (", "%s", argv[1], "%s" , ", ", "%s", argv[2], "%s", ")\n");
        exit_code = -1;
        goto prep_exit;
    }

    index = indexLoad((char *) indexFile);  // load an index using filename provided
    if (index == NULL) goto prep_exit;  // ensure index was created

    readParse(index, pageDir);

    prep_exit:  // exit prep that can be moved to from anypoint in the function to cover all bases
    if (pageDir != NULL) free(pageDir);
    if(indexFile != NULL) free(indexFile);
    if (index != NULL) indexDelete(index);
    return exit_code;
}

























