# Chu Hui Ong, CS50 Winter 2024
# Makefile for 'crawler' module: build the crawler executable program by default
# with a clean target that removes files produced by Make

PROG = crawler
OBJS = crawler.o 
# LIBS = ../libcs50/memory.o ../libcs50/bag.o ../libcs50/hashtable.o ../libcs50/webpage.o ../libcs50/set.o ../libcs50/jhash.o ../libcs50/file.o
LIBS = ../libcs50/libcs50-given.a ../common/common.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../common -I../libcs50
CC = gcc
MAKE = make

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# crawler.o: ../libcs50/bag.h ../libcs50/hashtable.h ../libcs50/webpage.h ../libcs50/memory.h \
 ../common/pagedir.h
# readline.o: ../libcs50/readlinep.h

.PHONY: test valgrind clean

test:
#	bash -v testing.sh
	bash testing.sh &> testing.out

# valgrind: $(PROG) 
# 	valgrind ./$(PROG) 

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(PROG)
	rm -rf ../data* ../garbage*