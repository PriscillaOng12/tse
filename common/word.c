/**
 * Chu Hui Ong, CS50 Winter 2024
 * 
 * word.c -- a module to normalize words by converting them to lowercase
 *
*/

#include <ctype.h>
#include <string.h>

int normalizeWord(char *word) {
    if (!word) { // Check for NULL pointer to ensure valid input
        return -1; // Return error code if input is invalid
    }

    // Iterate through each character of the word
    for (int i = 0, limit = strlen(word); i < limit; i++) {
        // Convert to lowercase only if it's an alphabetic character
        if (isalpha(word[i])) {
            word[i] = tolower(word[i]);
        }
    }

    return 0; // Indicate success
}