#ifndef PERFT_H
#define PERFT_H

uint64_t perft(Position &pos, int depth, bool divide);
void run_perft(Position &pos, int depth);
void run_perft_tests();

#endif
