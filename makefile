# comment
# (note: the <tab> in the command line is necessary for make to work) 
# target:  dependency1 dependency2 ...
#       <tab> command

# uncomment if needed
# CFLAGS = -std=c++11

# uncomment to compile with optimizations (big performance boost)
# OPTFLAGS = -Ofast -flto

all: 
	g++ -std=c++11 -Wall chess.cpp position.cpp movegen.cpp -o cppchess

release:
	g++ -std=c++11 -Ofast -flto chess.cpp position.cpp movegen.cpp -o cppchess


# cppchess: chess.o position.o movegen.o
# 	g++ $(CFLAGS) chess.o position.o movegen.o -o cppchess

# chess.o: chess.cpp
# 	g++ $(CFLAGS) -c chess.cpp

# position.o: position.cpp position.h
# 	g++ $(CFLAGS) -c position.cpp

# movegen.o: movegen.cpp movegen.h
# 	g++ $(CFLAGS) -c movegen.cpp

run:
	./cppchess

clean:
	rm *.o