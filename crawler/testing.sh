#!/bin/bash
#
# Chu Hui Ong, CS50 Winter 2024
#
# Testing script for crawler.c
# Usage: make test
# Input: none
# Output: Testing results, see testing.out for more specific results
#Verification Method: "Manual verification of files -- files match expectations"

#ARGUMENTS: ./crawler seedURL pageDirectory maxDepth
#example: ./crawler http://cs50tse.cs.dartmouth.edu/tse/ ./data/ 2

#******************* MAKE *.OUT FILE EXECUTABLE && DEFINE `GLOBAL` VARS**************************#

chmod +x testing.sh # change mode of .out file to executable

#  make parent directory outside cralwer to hold all new directories made in this test
mkdir ../tse-output

make


echo "Initializing Test ..."

echo "Integration testing for crawler module."
echo "Currently not using -DTEST flag to see progress indicators (e.g. file ID and URL)."
echo "(Please comment out for brevity, see Makefile in common to delete the flag)"
echo "Please use valgrind to check for memory leaks"

echo "=================================================================================="

#************************************* various commandline arguments testing block ************************************#
### The tests cases defined in this block should  all fail ###
echo "Section 0: Testing: Parameter Checks"
echo " Expect /tse-output/argstest-depth to be empty / have no files after completing this section tests "
echo "Error-handling: Insufficient/ Too many  arguments"

# 1 argument 
 echo " Error Handling with one argument"
 ./crawler
 if [ $? -ne 0 ];then
   echo >&2 "Error : Insufficient arguments provided"
 fi
 echo # Blank line

# 2 arguments
 echo " Error Handling with two argument"
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 if [ $? -ne 0 ];then
 	echo >&2 "Error : Insufficient arguments provided"
 fi

 mkdir ../tse-output/argstest-depth
 echo # Blank line

# 3 arguments
 echo " Error Handling with three argument"
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../tse-output/argstest-depth
 if [ $? -ne 0 ];then
    echo >&2 "Error: Insufficient arguments provided"
 fi
 echo # Blank line

# 4 arguments + externalURL
 echo "Error-handling: Testing crawler on invalid site (external)"
 ./crawler http://www.google.com/ ../tse-output/argstest-depth 2
 if [ $? -ne 0 ]; then
 	echo >&2 "Error caught: invalid site (external)"
 fi
 echo # Blank line

# 4 arguments + non-existent server
 echo "Error-handling: Testing crawler on invalid server (non-existent)"
 echo " Expect  number of files in ../tse-output/argstest-depth to be 0 "
 ./crawler http://abcd.com/notyetexist.php ../tse-output/argstest-depth 2
 if [ $? -ne 0 ]; then
    echo >&2 "Error : n invalid server (non-existent"
 fi
 ls ../tse-output/argstest-depth/ | wc -l
 echo # Blank line

# 4 arguments + valid server, non-existent page
 echo "Error-handling: Testing crawler on a valid server but non-existent page"
 echo " Expect  number of files in ../tse-output/argstest-depth  at depth 2 to be 0 "
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/salie ../tse-output/argstest-depth 2 # URL from Marlee Montella
 if [ $? -ne 0 ]; then
    echo >&2 "Error : n a valid server but non-existent page"
 fi
 ls ../tse-output/argstest-depth/ | wc -l
 echo # Blank line

# 4 arguments + negative page depth
 echo "Error-handling: Testing crawler on invalid depth (negative depth)"
 " Expect  number of files in ../tse-output/argstest-depth  at depth -10 to be 0 "
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../tse-output/argstest-depth -10
 if [ $? -ne 0 ]; then
    echo >&2 "Error : invalid depth (negative depth)"
 fi
 ls ../tse-output/argstest-depth/ | wc -l
 echo # Blank line

# Invalid directory - NULL 
 echo "Error-handling: Testing crawler on invalid directory (NULL directory)"
 echo " Expect NULL directory error message."
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ NULL 2
 if [ $? -ne 0 ]; then
    echo >&2 " Null directory not acceptable"
 fi
 echo # Blank line

# non-exisitent directory
 echo "Error-handling: Testing crawler on invalid directory (doesn't exist)"
 cho " Expect  non-existent directory error message."
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ nonexistent/ 2
 if [ $? -ne 0 ]; then
    echo  >&2 "Error: non exisitent directory not acceptable"
 fi
 echo # Blank line

# not writeable directory
 mkdir  ../tse-output/notWriteable
 chmod -w ../tse-output/notWriteable

 echo "Error-handling: Testing crawler on invalid directory (no write permission)"
 echo " Expect directory name ../tse-output/notWriteable  to be empty . Number of file should = 0"
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../tse-output/notWriteable 2
 if [ $? -ne 0 ]; then
    echo >&2 "Error : Directory must be writable"
 fi

 ls ../tse-output/notWriteable/ | wc -l
 echo # Blank line


chmod +w ../tse-output/notWriteable/

#*********************************************Regular(valid) cases testing block **************************************#
echo "=================================================================================="
echo
### The tests cases defined in this block should  all pass ###
echo "Section 1 Testing: Simple tests & webpages with cycles. Should not produce duplicate  or missing files."
echo

# simple html 
mkdir ../tse-output/simple-depth-2
echo " Crawling a simple html "
echo " Expect 0 file from simple html at depth 2"
./crawler http://cs50tse.cs.dartmouth.edu/tse/index.html ../tse-output/simple-depth-2 2 
if [ $? -ne 0 ];then
 	echo >&2 "Failed test at on simple html at depth = 2 "  
     exit 1
 fi

ls ../tse-output/simple-depth-2/ | wc -l

# closed set of cross-linked pages

echo
mkdir ../tse-output/crossletters-depth-4
echo "Crawling a simple, closed set of cross-linked web pages"
echo "Testing letters html at depth 4 (contains loops). Expect 8 files. "
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../tse-output/crossletters-depth-4 4 
 if [ $? -ne 0 ];then
 	echo  >&2 "Error: Failed test of a closed set of cross-linked web pages"
    exit 1
 fi

ls ../tse-output/crossletters-depth-4/ | wc -l

echo
echo "=================================================================================="
echo

# Exploring at depths 0, 1, 2, 3, 4, 5 and verifying
echo "Section 2 Testing: Exploring at depths 0, 1, 2, 3, 4, 5 and verifying"

# at depth 0
echo
mkdir ../tse-output/letters-depth-0
echo "Testing crawler on Letters at depth 0"
echo "Expect 1 file from Letters html at Depth 0."

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../tse-output/letters-depth-0 0
if [ $? -ne 0 ];then
	echo >&2 "Error Failed test on Letters at depth 0"
    exit 1
fi

ls ../tse-output/letters-depth-0/ | wc -l


# at depth 1
echo
mkdir ../tse-output/letters-depth-1
echo "Now crawling Letters html at Depth 1"
echo "Expect 2 files from Letters html at Depth 1 "
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../tse-output/letters-depth-1 1
 if [ $? -ne 0 ]; then
    echo >&2 "Error Failed test on Letters at depth 1"
    exit 1
 fi

ls ../tse-output/letters-depth-1/ | wc -l

# at depth 2
echo
mkdir ../tse-output/letters-depth-2
echo "Now crawling Letters html at Depth 2"
echo "Expect 3 files from Letters html at Depth 2 "
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../tse-output/letters-depth-2 2
 if [ $? -ne 0 ]; then 
    echo >&2 "Error Failed test on Letters at depth 2"
     exit 1
 fi

ls ../tse-output/letters-depth-2/ | wc -l

# # at depth 3
echo
mkdir ../tse-output/letters-depth-3
echo "Now crawling Letters html at Depth 3"
echo "Expect 6 files from Letters html at Depth 3 "
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../tse-output/letters-depth-3 3
 if [ $? -ne 0 ]; then 
    echo >&2 "Error Failed test on Letters at depth 3"
    exit 1
 fi

ls ../tse-output/letters-depth-3/ | wc -l

# # at depth 4
echo
mkdir ../tse-output/letters-depth-4
echo "Now crawling Letters html at Depth 4 "
echo "Expect 8 files from Letters html at Depth 4 "
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../tse-output/letters-depth-4 4 
 if [ $? -ne 0 ]; then
   echo >&2 "Error Failed test on Letters at depth 4"
    exit 1
 fi

ls ../tse-output/letters-depth-4/ | wc -l

echo
echo "================================================================================================"
echo

echo "Section  3 Testing cont.: Repeating with a different seed page in the same site"

 # at B html at depth 0
 echo
 mkdir ../tse-output/BHTML-depth-0
 echo "Now crawling B html at Depth 0"
 echo "Expect 1 files from B html at Depth 0:"
 
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/B.html ../tse-output/BHTML-depth-0 0
 if [ $? -ne 0 ]; then
    echo >&2 "Error Failed test on B html at Depth 0"
    exit 1
 fi

ls ../tse-output/BHTML-depth-0/ | wc -l

# at B html at depth 1
 echo
 mkdir ../tse-output/BHTML-depth-1
 echo "Now crawling B html at Depth 1"
 echo "Expect 5 files from B html at Depth 1 : "
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/B.html ../tse-output/BHTML-depth-1 1
 if [ $? -ne 0 ]; then
    echo >&2 "Error Failed test on B html at Depth 1"
    exit 1
 fi
 
 ls ../tse-output/BHTML-depth-1/ | wc -l

 # at B html at depth 2
 echo
 mkdir ../tse-output/BHTML-depth-2
 echo "Now crawling B html at Depth 2"
 echo "Expect 8 files from B html at Depth 2 "
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/B.html ../tse-output/BHTML-depth-2 2
 if [ $? -ne 0 ]; then
    echo >&2 "Error Failed test on B html at Depth 2"
    exit 1
 fi

ls ../tse-output/BHTML-depth-2/ | wc -l

 # at B html at depth 3
 echo
 mkdir ../tse-output/BHTML-depth-3
 echo "Now crawling B html at Depth 3"
 echo "Expect 9 files from B html at Depth 3"
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/B.html ../tse-output/BHTML-depth-3 3
 if [ $? -ne 0 ]; then
    echo >&2 "Error Failed test on B html at Depth 3"
    exit 1
 fi

ls ../tse-output/BHTML-depth-3/ | wc -l

# at B html at depth 4
 echo
 mkdir ../tse-output/BHTML-depth-4
 echo "Now crawling B html at Depth 4"
 echo "Expect 9 files from B html at Depth 4"
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/B.html ../tse-output/BHTML-depth-4 4
 if [ $? -ne 0 ]; then
    echo >&2 "Error Failed test on B html at Depth 4"
    exit 1
 fi

ls ../tse-output/BHTML-depth-4/ | wc -l

 # at B html at depth 5
 echo
 mkdir ../tse-output/BHTML-depth-5
 echo "Now crawling B html at Depth 5"
 echo "Expect 9 files from B html at Depth 5"
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/B.html ../tse-output/BHTML-depth-5 5
 if [ $? -ne 0 ]; then
    echo >&2 "Error Failed test on B html at Depth 5"
    exit 1
 fi

 ls ../tse-output/BHTML-depth-5/ | wc -l

echo
echo "================================================================================="
echo

## Exploring other  pages
echo "Section 4:  Testing: Additional tests"

 # toscrape at level 0
 echo
mkdir ../tse-output/toscrape-depth-0
echo "Testing crawler on toscrape html file at depth 0"
echo "Expect 1 file from toscrape html at Depth 0."

 ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../tse-output/toscrape-depth-0 0
 if [ $? -ne 0 ]; then 
   echo >&2 "Error Failed test on toscrape html at Depth 0"
   exit 1
 fi

ls ../tse-output/toscrape-depth-0/ | wc -l


 # at depth 1
 echo
mkdir ../tse-output/toscrape-depth-1
echo "Testing crawler on toscrape html file at depth 1"
echo "Expect 73 file from toscrape html at Depth 1."

 ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../tse-output/toscrape-depth-1 1
 if [ $? -ne 0 ]; then
	echo >&2 "Error Failed test on toscrape html at Depth 1"
    exit 1
 fi

ls ../tse-output/toscrape-depth-1/ | wc -l

 # at depth 2
echo
mkdir ../tse-output/toscrape-depth-2
echo "Testing crawler on toscrape html file at depth 2"
echo "Expect 585 file from toscrape html at Depth 2."
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../tse-output/toscrape-depth-2 2
 if [ $? -ne0 ]; then
 	echo >&2 "Error Failed test on toscrape html at Depth 2"
    exit 1
 fi

ls ../tse-output/toscrape-depth-2/ | wc -l

#  # at depth 3
# echo
# mkdir ../tse-output/toscrape-depth-3
# echo "Testing crawler on toscrape html file at depth 3"
# echo "Expect 585 file from toscrape html at Depth 3."
#  ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../tse-output/toscrape-depth-3 3
#  if [ $? -ne0 ]; then
#  	echo >&2 "Error Failed test on toscrape html at Depth 3"
#     exit 1
#  fi

# ls ../tse-output/toscrape-depth-3/ | wc -l

echo
echo "=============================================================================="
echo
# ## Crawling Wikipedia playground. Exploring at depths 0, 1, and 2

echo "Section 5 Testing: Wikipedia playground at depths 0, 1, 2"

# at depth 0
echo
mkdir ../tse-output/wikipedia-depth-0
echo "Testing crawler on wikipedia html file at depth 0"
echo "Expect 1 file from Wikipedia html at Depth 0."
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../tse-output/wikipedia-depth-0 0
 if [ $? -ne 0 ]; then
  echo >&2 "Error Failed test on B html at Depth 0"
  exit 1
 fi

ls ../tse-output/wikipedia-depth-0/ | wc -l

# at depth 1
echo
mkdir ../tse-output/wikipedia-depth-1
echo "Testing crawler on wikipedia html file at depth 1"
echo "Expect 7 file from Wikipedia html at Depth 1."
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../tse-output/wikipedia-depth-1 1
 if [ $? -ne 0 ]; then
  echo >&2 "Error Failed test on B html at Depth 1"
  exit 1
fi

ls ../tse-output/wikipedia-depth-1/ | wc -l

#  # at depth 2
# echo
# mkdir ../tse-output/wikipedia-depth-2
# echo "Testing crawler on wikipedia html file at depth 2"
# echo "Expect 1673 file from Wikipedia html at Depth 2."
#  ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../tse-output/wikipedia-depth-2 2
#  if [ $? -ne 0 ]; then
#     echo >&2 "Error Failed test on wikipedia html at Depth 2"
#     exit 1
#   fi

# ls ../tse-output/wikipedia-depth-2/ | wc -l

# report end of testing
echo
echo "================================================================================="
echo "Section 6:  Reporting  end of testing "

echo " Testing Complete."

