# Chu Hui Ong, CS50 Winter 2024
# Makefile for 'querier' module: build the querier executable by default
# with a make clean target that removes files produced by Make
# use `make test` to test querier

.PHONY: all test valgrind clean

C = ../common
L = ../libcs50
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
LLIBS = $C/common.a $L/libcs50-given.a
LIBS =

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all -s

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L -I$C
CC = gcc
MAKE = make

all: querier fuzzquery

querier: querier.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

fuzzquery: fuzzquery.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# querier source dependencies
querier.o:  $C/word.h $C/index.h $L/mem.h $L/webpage.h $L/file.h

# fuzzquery source dependencies
fuzzquery.o:  $L/mem.h

# expects crawler directories  (from cs50dev shared output files)
# ../../shared/tse/output/letters-1  ../../shared/tse/output/letters-2  ../../shared/tse/output/letters-3
# ../../shared/tse/output/toscrape-1 ../../shared/tse/output/toscrape-2 ../../shared/tse/output/toscrape-3
# ../../shared/tse/output/wikipedia-1 ../../shared/tse/output/wikipedia-2 ../../shared/tse/output/wikipedia-3
test: querier fuzzquery testing.sh 
	bash -v testing.sh 

# expects directory (from cs50dev shared output files)
# ../../shared/tse/output/letters-3 ../../shared/tse/output/toscrape-3 ../../shared/tse/output/wikipedia-3
valgrind: querier
	$(VALGRIND) ./querier ../../shared/tse/output/letters-3 ../../shared/tse/output/letters-3.index
	$(VALGRIND) ./querier ../../shared/tse/output/toscrape-3 ../../shared/tse/output/toscrape-3.index
	$(VALGRIND) ./querier ../../shared/tse/output/wikipedia-3 ../../shared/tse/output/wikipedia-3.index

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f querier
	rm -f fuzzquery
	rm -f core
	# rm -f {letters,toscrape,wikipedia}-*-{basic,fuzzquery}-test.out  # Remove test output files


