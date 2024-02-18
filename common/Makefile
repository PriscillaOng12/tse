# Chu Hui Ong, CS50 Winter 2024
# Makefile for 'common' module: build the common.a lib by default
# with a clean target that removes files produced by Make

# object files, and the target library
OBJS = pagedir.o word.o index.o
LIB = common.a
LIBS = ../libcs50/libcs50-given.a
FLAGS = -lm
INDEXCOEFF = -DINDEXCOEFF
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TEST) $(FLAGS) -I../libcs50/
CC = gcc
TEST = 
MAKE = make

# Build $(LIB) by archiving object files
# (you will need to drop in copy of ????)
$(LIB): $(OBJS) $(LIBS)
	ar cr $(LIB) $(OBJS)


pagedir.o: pagedir.c pagedir.h
index.o: index.c index.h word.o
word.o: word.c word.h

all: $(LIB)

valgrind:
	clear; make clean; make;
	# $(VALGRIND)
	
.PHONY: all clean test valgrind


# clean up after our compilation
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f core
	rm -f $(LIB) *~ *.o