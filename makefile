# comment
# (note: the <tab> in the command line is necessary for make to work) 
# target:  dependency1 dependency2 ...
#       <tab> command

# uncomment if needed
#CFLAGS = -std=c++11

# uncomment to compile with optimizations (big performance boost)
CFLAGS = -Ofast -flto

all: chess

chess: chess.o position.o movegen.o
	g++ $(CFLAGS) chess.o position.o movegen.o -o chess

chess.o: chess.cpp
	g++ $(CFLAGS) -c chess.cpp

position.o: position.cpp position.h
	g++ $(CFLAGS) -c position.cpp

movegen.o: movegen.cpp movegen.h
	g++ $(CFLAGS) -c movegen.cpp

run:
	./chess

clean:
	rm *.o