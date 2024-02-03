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