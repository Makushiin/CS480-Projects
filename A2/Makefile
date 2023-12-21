#Name : Aya Abdul Hasan
#RedID : 824080459

#Name : Maxine Mayor
#RedID : 825551461

# CXX Make variable for compiler
CC=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CCFLAGS=-std=c++11 -Wall -g3 -c

# object files
OBJS = tree.o readvocab.o readlines.o countvocabstrings.o main.o 

# Program name
PROGRAM = countvocabstrings

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -pthread -o $(PROGRAM) $(OBJS)

main.o : main.cpp
	$(CC) $(CCFLAGS) main.cpp
	
tree.o : tree.cpp tree.h
	$(CC) $(CCFLAGS) tree.cpp

readvocab.o : readvocab.cpp readvocab.h
	$(CC) $(CCFLAGS) readvocab.cpp

readlines.o: readlines.cpp readlines.h
	$(CC) $(CCFLAGS) readlines.cpp
    
countvocabstrings.o: countvocabstrings.cpp countvocabstrings.h
	$(CC) $(CCFLAGS) countvocabstrings.cpp

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f *.o *~ $(PROGRAM)

