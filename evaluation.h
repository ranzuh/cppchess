#ifndef EVALUATION_H
#define EVALUATION_H

int evaluate_position(Position &pos);
int evaluate_position2(Position &pos);
int eval_white_pawn(int square, int rank, int file);
int eval_black_pawn(int square, int rank, int file);

#endif