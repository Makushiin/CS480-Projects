
#Zizhao Zheng 825807834
#Maxine Mayor 825551461

# CXX Make variable for compiler
CC = g++
CFLAGS = -std=c++11 -Wall -g3 -c
CCOMPILE = gcc
CCFLAGS = -std=c99 -Wall -g3 -c

OBJS =  pageTable.o WSClock.o log_helpers.o vaddr_tracereader.o main.o

PROGRAM = demandpaging

$(PROGRAM) : $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS)

main.o : main.cpp
	$(CC) $(CCFLAGS) main.cpp

WSClock.o: WSClock.cpp WSClock.h
	$(CC) $(CCFLAGS) WSClock.cpp

pageTable.o: pageTable.cpp pageTable.h
	$(CC) $(CCFLAGS) pageTable.cpp

log_helpers.o: log_helpers.c log_helpers.h
	$(CC) $(CCFLAGS) log_helpers.c

vaddr_tracereader.o: vaddr_tracereader.c vaddr_tracereader.h
	$(CC) $(CCFLAGS) vaddr_tracereader.c

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f *.o *~ $(PROGRAM)

