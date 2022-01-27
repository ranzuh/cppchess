# C++ Chess engine

## Features
* [0x88](https://www.chessprogramming.org/0x88) board representation
* Negamax search with alpha-beta pruning
* Quiescence search
* Iterative deepening
* Transposition table
* Evaluation using piece-square tables and MVV-LVA
* Principal variation table
* 3-fold repetition handling
* Uses UCI to communicate with a Chess GUI

(development) compile and run with:
```
g++ -std=c++11 chess.cpp position.cpp movegen.cpp -o chess.exe 
.\chess.exe
```

(release) compile and run with:
```
g++ -std=c++11 -Ofast -flto chess.cpp position.cpp movegen.cpp -o chess.exe
.\chess.exe
```
