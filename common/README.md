# CS50 Lab 4 Crawler 
### common

The `common` directory assembles code that will eventually be shared by the crawler, indexer, and querier. For now, that comprises code for initializing the "pageDirectory" and saving webpages there.

### pagedir
We create a re-usable module pagedir.c to handles the pagesaver mentioned in the design (writing a page to the pageDirectory), and marking it as a Crawler-produced pageDirectory (as required in the spec). We chose to write this as a separate module, in ../common, to encapsulate all the knowledge about how to initialize and validate a pageDirectory, and how to write and read page files, in one place... anticipating future use by the Indexer and Querier.

### Usage

The *pagedir* module, defined in `pagedir.h` and implemented in `pagedir.c`, checks whether a passed directory is valid (exists & writable), and saves and fetches webpages. These are implemented with the below methods:

```c
bool isDirectory(char* directory);
char* pagefetcher(webpage_t *page);
bool pagesaver(webpage_t *page, char* pageDir, int id);
```

### index
The 'index' module defines a data structure that maps words to (document ID, count) pairs, where each word is associated with multiple document IDs and each document ID has a count of how many times the word appears in that document. This module provides functionality to create, manipulate, save, load, and delete an index, as well as to perform searches within it.

```c
index_t* index_new(const int size);
void index_save(index_t* index, char* indexFilename);
void index_load(index_t* index, char* indexFilename);
void index_delete(index_t* index);
void index_search(index_t* index, char** words, int* scores, int numDocs, int numWords);

counters_t* index_find(index_t* index, char* key);
bool index_add(index_t* index, char* key, int docID);
int num_docs_crawled(char* pageDirectory);

static void index_itr(void* fp, const char* key, void* item);
static void index_itr_helper(void* fp, const int key, const int count);
static void index_delete_helper(void* item);
```