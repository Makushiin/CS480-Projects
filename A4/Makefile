# Maxine Mayor
# 825551461

# CXX Make variable for compiler
CXX=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -Wall -g3 -c

# object files
OBJS = report.o tradecrypto.o

# Program name
PROGRAM = tradecrypto

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
$(PROGRAM) : $(OBJS)
	$(CXX) -pthread -o $(PROGRAM) $^

report.o : tradecrypto.h report.h report.cpp
	$(CXX) $(CXXFLAGS) report.cpp

tradecrypto.o : report.o consumer.h producer.h shared.h tradecrypto.h tradecrypto.cpp
	$(CXX) $(CXXFLAGS) tradecrypto.cpp

clean :
	rm -f *.o *.exe $(PROGRAM)
