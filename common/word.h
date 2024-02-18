/**
 * Chu Hui Ong, CS50 Winter 2024
 * 
 * word.h -- header file for CS50 'word' module
 * 
 */

/**
 * Normalize a word by converting all its characters to lowercase.
 * 
 * This function iterates over each character of the input word. If a character
 * is an alphabet letter, it converts it to lowercase. This function is used to
 * preprocess words for text processing tasks, ensuring that word comparisons
 * are case-insensitive.
 *
 * @param word The word to be normalized. This must be a null-terminated string.
 * @return 0 if the word was successfully normalized, -1 if the input is NULL.
 */
int normalizeWord(char *word);