CXX = clang++
CXXFLAGS = -std=c++11 -Wall
CXXRELEASE = -std=c++11 -O3 -Winline
EXECUTABLE = cppchess

OBJECTS = main.o position.o movegen.o hashtable.o uci.o search.o evaluation.o perft.o linear_evaluation.o tdleaf.o
SOURCES = main.cpp position.cpp movegen.cpp hashtable.cpp uci.cpp search.cpp evaluation.cpp perft.cpp linear_evaluation.cpp tdleaf.cpp

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $^

release:
	$(CXX) $(CXXRELEASE) -o $(EXECUTABLE) $(SOURCES)

$(OBJECTS): position.h movegen.h hashtable.h uci.h search.h evaluation.h perft.h linear_evaluation.h


# Best for g++ compiler
# release:
# 	g++ -std=c++11 -O3 -flto main.cpp position.cpp movegen.cpp hashtable.cpp uci.cpp search.cpp evaluation.cpp perft.cpp -o cppchess -Winline


run:
	./cppchess

clean:
	rm *.o