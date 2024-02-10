# Chu Hui Ong, CS50 Winter 2024
# Makefile for 'common' module: build the common.a lib by default
# with a clean target that removes files produced by Make
 
# PROG = pagedir
OBJS = pagedir.o index.o
LIBS = ../libcs50/libcs50-given.a
LIB = common.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I ../libcs50
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# common.o: ../libcs50/libcs50-given.a pagedir.h
# readline.o: ../libcs50/readlinep.h
pagedir.o: pagedir.h
index.o: index.h


.PHONY: test valgrind clean

valgrind: $(PROG) 
	valgrind ./$(PROG) 

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(PROG)