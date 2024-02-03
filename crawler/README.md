# CS50 Lab 4 Crawler 
### crawler

A `crawler` crawl a website and retrieve webpages beginning with a given URL. It parses through the initial webpage, extracts embedded URLs and retrieves those pages, and crawls these pages until there are no more pages to crawl or `maxDepth` is reached. Crawled URLs are further limited by those 'internal' to the designated CS50 server. When the crawler process is complete, the indexing of the documents begin, in which each document is saved to a unique id, with URL, current crawled depth, and HTML content.

### Usage

The Crawler is implemented in one file crawler.c, with five functions:

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
static void logr(const char *word, const int depth, const char *url); 
```
I added the `logr` function to provide detailed logging of the crawler's actions, including fetched URLs, scanned URLs, ignored external URLs, and duplicate URLs. 

### Testing

The `testing.sh` script tests crawler on numerous files, and tests errors and edge cases. Test scripts for toscrape at depth 3 and wikipedia at depth 2 are commented out to prevent extremely large testing.out