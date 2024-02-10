.PHONY: all test valgrind clean

C = ../common
L = ../libcs50
CC = gcc
MAKE = make

# Combined and corrected CFLAGS definition
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C

# Linking libraries
LLIBS = $C/common.a $L/libcs50-given.a
LIBS =

# For memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all -s

# Default target
all: indexer indextest

# Building indexer
indexer: indexer.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Building indextest
indextest: indextest.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Source file dependencies
indexer.o: $C/index.h $L/mem.h $L/webpage.h
indextest.o: $C/index.h $L/file.h indexer.c

# Testing target
test: indexer indextest testing.sh
	bash -v testing.sh >& testing.out
	# bash testing.sh >& testing.out 

# Memory-leak tests with Valgrind
valgrind: indexer indextest
	$(VALGRIND) ./indexer ../tse-output/letters-depth-1 ../tse-output/letters-depth-1/index.ndx
	$(VALGRIND) ./indextest ../tse-output/letters-depth-1/index.ndx ../tse-output/letters-depth-1/index_new.ndx

# Clean up
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f indexer
	rm -f indextest
	rm -f core
	# rm -f testing.out  # Cleaning up test output
