# C++ Chess engine

Chess engine written in C++. Needs UCI-compliant Chess GUI to play against it, for example [Arena](http://www.playwitharena.de/) or [Cute Chess](https://cutechess.com/).

## Features
* [0x88](https://www.chessprogramming.org/0x88) board representation
* Negamax search with alpha-beta pruning
* Quiescence search
* Iterative deepening
* Transposition table
* Evaluation using material, piece-square tables and basic pawn/rook structures
* Principal variation table
* Move ordering with MVV-LVA, PV-nodes and killer moves
* 3-fold repetition handling
* Null move pruning
* Late move reductions
* PV-search

## Setup

For development without optimization flags
compile and run with:
```
make
make run
```

For release compile and run with:
```
make release
make run
```

After compiling you can select the binary cppchess (or cppchess.exe in Windows) in your chess-GUI of choice.

## Thanks
This engine started with me following Maksim Korzh's youtube video series [0x88 MOVE GENERATOR](https://www.youtube.com/playlist?list=PLmN0neTso3JzhJP35hwPHJi4FZgw5Ior0) and [Bitboard CHESS ENGINE in C](https://www.youtube.com/playlist?list=PLmN0neTso3Jxh8ZIylk74JpwfiWNI76Cs) but now it has started to evolve into its own thing. I also took inspiration from [Blunder](https://github.com/algerbrex/blunder) and [TSCP](http://www.tckerrigan.com/Chess/TSCP/).
