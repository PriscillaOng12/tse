# Chu Hui Ong, CS50 Winter 2024
# test cases for querier.c

#! /bin/bash

echo "Integration testing for querier module."
echo "Valgrind included in the final part to check for memory leaks"
echo "=========================================================="
echo

### Test indexer with various invalid arguments ###
echo -e "\n### testing with various arguments ###"

# no arguments
echo -e "\ntesting with no arguments"
./querier
echo

# one argument
echo -e "\ntesting with one argument"
./querier ../../shared/tse/output/letters-1
echo

# three or more arguments
echo -e "\ntesting with three or more arguments"
./querier ../../shared/tse/output/letters-1 arg2 arg3
./querier ../../shared/tse/output/letters-1 arg2 arg3 arg4 arg5 arg6 arg7 arg8
echo

# invalid pageDirectory
echo -e "\ntesting with invalid pageDirectory (non-existent path)"
./querier ../../shared/tse/output/letters-100 ../../shared/tse/output/letters-100.index
echo

# pageDirectory not a crawler directory
echo -e "\ntesting with invalid pageDirectory (not a crawler directory)"
mkdir ../../shared/tse/output/not_crawler
touch ../../shared/tse/output/not_crawler/not_crawled.txt
./querier ../../shared/tse/output/not_crawler ../../shared/tse/output/not_crawler.index
# check if extra file created
FILE=../../shared/tse/output/not_crawler.index
if [[ -f "$FILE" ]]; then
    echo -e "\nERROR: $FILE created"
fi
# cleanup directory
rm -r ../../shared/tse/output/not_crawler 

# invalid indexFile (non-existent path)
echo -e "\ntesting with invalid indexFile (non-existent path)"
./querier ../../shared/tse/output/letters-3 ../../shared/tse/output/letters-100.index

# valid testings on letters, toscrape, and wikipedia
# testing on letters-1
pdir="../../shared/tse/output/letters-1"
indx="../../shared/tse/output/letters-1.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch letters-1-basic-test.out
./querier $pdir $indx < BASIC_TEST >& letters-1-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch letters-1-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& letters-1-fuzzquery-test.out


# testing on letters-2
pdir="../../shared/tse/output/letters-2"
indx="../../shared/tse/output/letters-2.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch letters-2-basic-test.out
./querier $pdir $indx < BASIC_TEST >& letters-2-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch letters-2-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& letters-2-fuzzquery-test.out


# testing on letters-3
pdir="../../shared/tse/output/letters-3"
indx="../../shared/tse/output/letters-3.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch letters-3-basic-test.out
./querier $pdir $indx < BASIC_TEST >& letters-3-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch letters-3-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& letters-3-fuzzquery-test.out


# testing on toscrape-1
pdir="../../shared/tse/output/toscrape-1"
indx="../../shared/tse/output/toscrape-1.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch toscrape-1-basic-test.out
./querier $pdir $indx < BASIC_TEST >& toscrape-1-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch toscrape-1-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& toscrape-1-fuzzquery-test.out


# testing on toscrape-2
pdir="../../shared/tse/output/toscrape-2"
indx="../../shared/tse/output/toscrape-2.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch toscrape-2-basic-test.out
./querier $pdir $indx < BASIC_TEST >& toscrape-2-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch toscrape-2-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& toscrape-2-fuzzquery-test.out


# testing on toscrape-3
pdir="../../shared/tse/output/toscrape-3"
indx="../../shared/tse/output/toscrape-3.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch toscrape-3-basic-test.out
./querier $pdir $indx < BASIC_TEST >& toscrape-3-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch toscrape-3-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& toscrape-3-fuzzquery-test.out


# testing on wikipedia-1
pdir="../../shared/tse/output/wikipedia-1"
indx="../../shared/tse/output/wikipedia-1.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch wikipedia-1-basic-test.out
./querier $pdir $indx < BASIC_TEST >& wikipedia-1-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch wikipedia-1-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& wikipedia-1-fuzzquery-test.out

# testing on wikipedia-2
pdir="../../shared/tse/output/wikipedia-2"
indx="../../shared/tse/output/wikipedia-2.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch wikipedia-2-basic-test.out
./querier $pdir $indx < BASIC_TEST >& wikipedia-2-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch wikipedia-2-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& wikipedia-2-fuzzquery-test.out

# testing on wikipedia-3
pdir="../../shared/tse/output/wikipedia-3"
indx="../../shared/tse/output/wikipedia-3.index"
echo -e "\nBasic testing on pageDirectory: $pdir"
touch wikipedia-3-basic-test.out
./querier $pdir $indx < BASIC_TEST >& wikipedia-3-basic-test.out


echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch wikipedia-3-fuzzquery-test.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& wikipedia-3-fuzzquery-test.out


# ### Run valgrind on querier to ensure no memory leaks or errors ###
echo -e "\nrunning valgrind in querier for letters at depth 3 to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./querier ../../shared/tse/output/letters-3 ../../shared/tse/output/letters-3.index < BASIC_TEST

echo -e "\nrunning valgrind in querier for toscrape at depth 3 to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./querier ../../shared/tse/output/toscrape-3 ../../shared/tse/output/toscrape-3.index < BASIC_TEST

echo -e "\nrunning valgrind in querier for wikipedia at depth 2 to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./querier ../../shared/tse/output/wikipedia-2 ../../shared/tse/output/wikipedia-2.index < BASIC_TEST



