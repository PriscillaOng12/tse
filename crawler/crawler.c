/**
 * Chu Hui Ong, CS50 Winter 2024
 * 
 * crawler.c -- The TSE crawler is a standalone program that crawls the web 
 * and retrieves webpages starting from a "seed" URL. It parses the seed webpage, 
 * extracts any embedded URLs, then retrieves each of those pages, recursively, 
 * but limiting its exploration to a given "depth".
 * 
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../libcs50/set.h"
#include "../libcs50/hash.h"
#include "../libcs50/mem.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/bag.h"
#include "../libcs50/webpage.h"
#include "../common/pagedir.h"
#include <string.h>


/**********************function prototypes**********************/
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
static void logr(const char *word, const int depth, const char *url);  // helper for tracking crawling progress and debugging


/**********************main**********************/
int main (const int argc, char* argv[]) {
  // defensive code
  if (argc == 4) {
    // get maxDepth as an integer
    int maxDepth = atoi(argv[3]);
    // call parseArgs and crawl
    parseArgs(argc, argv, &argv[1], &argv[2], &maxDepth);
    char* URL = malloc(strlen(argv[1]) + 1);
    if (URL != NULL) {
      strcpy(URL, argv[1]);
    }
    crawl(URL, argv[2], maxDepth);
    free(URL);
    return 0;
  }
  return 1;
}

/**********************parseArgs**********************/
void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) {
  // if there are 4 arguments, continue on
  // otherwise exit with non-zero status
  if (argc == 4) {
    // normalize URL
    char *URL = normalizeURL(*seedURL);
    // if not internal URL then exit with non-zero status
    if (isInternalURL(URL) != true) {
      fprintf(stderr, "Not internal URL");
      exit(2);
    }
    // free the URL
    free(URL);
    bool exists = pagedir_init(*pageDirectory);
    // if directory does not exist then exit with non-zero status
    if (exists == false) {
      fprintf(stderr, "Unable to create .crawler file in pageDirectory");
      exit(6);
    }
    // if all of the above is true and maxDepth is in range [1, 10], then return otherwise exit with non-zero status
    if (0 <= *maxDepth && *maxDepth <= 10) {
      return;
    } else {
      fprintf(stderr, "Max depth should be between 0 and 10 (inclusive)");
      exit(3);
    }
  } else {
    fprintf(stderr, "Input 4 arguments");
    exit(1);
  }
}


/**********************crawl**********************/
void crawl(char* seedURL, char* pageDirectory, const int maxDepth) {
  // will be used to give file its name
  int id = 0;
  // create a new hashtable
  hashtable_t *ht = hashtable_new(200); //number of slots as specified by specs
  // insert the seedURL
  hashtable_insert(ht, normalizeURL(seedURL), "const");
  // create a new bag
  bag_t *bag = bag_new();
  // create a new webpage from the seedURL with depth 0 and null HTML
  webpage_t *wbp = webpage_new((normalizeURL(seedURL)), 0, NULL);
  // insert the webpage in the bag
  bag_insert(bag, wbp);
  // extract a webpage from the bag while the bag is not empty
  webpage_t *page;
  while ((page = bag_extract(bag)) != NULL) {
    // fetch the HTML for the webpage
    if (webpage_fetch(page)) {
      // print the log status
      logr("Fetched", webpage_getDepth(page), webpage_getURL(page));
      // save the webpage in pageDirectory
      pagedir_save(page, pageDirectory, ++id);
      // if the webpage's depth is less than the maxDepth
      if (webpage_getDepth(page) < maxDepth) {
        // scan the webpage
        logr("Scanning", webpage_getDepth(page), webpage_getURL(page));
        pageScan(page, bag, ht);
      }
    }
    // delete the webpage
    webpage_delete(page);
  }
  // delete the hashtable
  hashtable_delete(ht, NULL);
  // delete the bag
  bag_delete(bag, webpage_delete);
}


/**********************pageScan**********************/
void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) {
  int pos = 0;
  char* result;
  // get all the URLs on a page
  while ((result = webpage_getNextURL(page, &pos)) != NULL) {
    logr("Found", webpage_getDepth(page), result);
    // if URL is internal then continue
    // otherwise print extrn url, ignore
    if (isInternalURL(normalizeURL(result))) {
      // if unique URL, continue
      // otherwise print duplicate url, ignore
      bool isInHt = hashtable_insert(pagesSeen, normalizeURL(result), "const");
      if (isInHt) {
        char* URL = malloc(strlen(result) + 1);
        if (URL != NULL) {
          strcpy(URL, result);
        }
        // add the new page found on the current page with +1 depth to the bag
        int depth = webpage_getDepth(page) + 1;
        webpage_t *wbpg = webpage_new(normalizeURL(URL), depth, NULL);
        bag_insert(pagesToCrawl, wbpg);
        logr("Added", webpage_getDepth(page), result);
      } else {
        logr("IgnDupl", webpage_getDepth(page), result);
      }
    }
    else {
      logr("IgnExtn", webpage_getDepth(page), result);
    }
    // free the result string
    free(result);
  }
}


/**********************logr**********************/
static void logr(const char *word, const int depth, const char *url) {
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
}

