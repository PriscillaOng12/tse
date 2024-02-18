# Design Specification for the Querier Module

## Overview
The Querier module is designed to read the index produced by the Indexer and serve queries entered by the user. It interacts with a page directory created by the Crawler to match queries against the indexed documents, ranking them based on the number of occurrences of the query terms.

## Data Structures
### `lnode_t`
- Purpose: Stores a pair of integers in a linked list node.
- Components:
  - `int values[2]`: Stores a document ID and its corresponding score.
  - `struct lnode *next`: Points to the next node in the list.

### `iter_arg_t`
- Purpose: Passes multiple arguments to iteration functions.
- Components:
  - `counters_t *res`: Accumulates results during iterations.
  - `counters_t *other`: Holds auxiliary data for comparison or merging.
  - `lnode_t *argNode`: Points to the head of a linked list for sorting.
  - `int count`: Tracks the number of nodes in the linked list.

## Functions

### `parseArgs`
- Input: `args[]`, `pageDir`, `indexFile`
- Output: Status code (0 for success, -1 for failure)
- Pseudocode:
  1. Validate input arguments.
  2. Allocate and copy `pageDir` from `args[1]`. Remove trailing slash if present.
  3. Validate `pageDir` using `pageDirValidate`.
  4. Allocate and copy `indexFile` from `args[2]`.
  5. Attempt to open `indexFile` to confirm its existence.
  6. Return 0 on success; otherwise, clean up and return -1.

### `query`
- Input: `index`, `pageDir`, `queryList`
- Output: Status code (0 for failure, -1 for success)
- Pseudocode:
  1. Validate input arguments.
  2. Initialize a `bag_t` to hold discrete query blocks.
  3. Tokenize `queryList` and process each token:
     - For each word, find or create its counter in `index`.
     - Use `isOP` to determine logical operators.
     - Perform intersection or union as dictated by operators.
  4. Merge final results and print using `sortPrint`.
  5. Clean up and return status code.

### `readParse`
- Input: `index`, `pageDir`
- Output: None (side effect: processes user queries)
- Pseudocode:
  1. Prompt the user for queries until EOF.
  2. Validate and normalize each query.
  3. Tokenize the query and process it using `query`.

### `prompt`
- Output: A line of input from the user.
- Pseudocode:
  1. Display a prompt if in interactive mode.
  2. Read and return a line of input.

### `tokenize`
- Input: `list`, `line`
- Output: Status code (0 for success, -1 for failure)
- Pseudocode:
  1. Validate input arguments.
  2. Loop through `line`, identifying words.
  3. Normalize and store each word in `list`.
  4. Return 0 on success; otherwise, return -1.

### `isOP`
- Input: `word`
- Output: Boolean (true if `word` is a logical operator)
- Pseudocode:
  1. Return true if `word` matches "and" or "or"; otherwise, false.

### `validateQuery`
- Input: `query`, `querySize`
- Output: Boolean (true if the query is valid)
- Pseudocode:
  1. Check for operators at the start or end of `query`.
  2. Ensure no adjacent operators.
  3. Return true if all checks pass.

### `countersIntersect` and `countersUnion`
- Input for `countersIntersect`: `arg`, `key`, `val1`
- Input for `countersUnion`: `arg`, `key`, `val1`
- Output: None (side effect: updates `arg->res` counter)
- Pseudocode for `countersIntersect`:
  1. For each key in `arg->res`, find the minimum of `val1` and `arg->other[key]`.
  2. Update `arg->res[key]` to the minimum value.
- Pseudocode for `countersUnion`:
  1. For each key in `arg->res`, add `val1` to `arg->res[key]`.

### `sortPrint`
- Input: `scores`, `pageDir`
- Output: Status code (0 for success, -1 for failure)
- Pseudocode:
  1. Validate input arguments.
  2. Iterate through `scores`, sorting entries.
  3. Print each sorted entry with its document URL.

### `sortIterate` and `copyIter`
- Input for `sortIterate`: `arg`, `key`, `val`
- Input for `copyIter`: `arg`, `key`, `val`
- Output: None (side effect: modifies `arg` data structure)
- Pseudocode for `sortIterate`:
  1. Insert each `key-val` pair into a sorted linked list in `arg->argNode`.
- Pseudocode for `copyIter`:
  1. Copy each `key-val` pair from the source to the destination counter in `arg->res`.

### `logMessage`
- Input: `argc`, variadic arguments
- Output: None (side effect: prints log messages)
- Pseudocode:
  1. If in test mode, print each argument according to its format specifier.
