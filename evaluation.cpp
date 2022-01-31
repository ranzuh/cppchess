#include "position.h"
//#include "evaluation.h"
#include <iostream>

////////////////
// evaluation //
////////////////

// e, p, n, b, r, q, k, P, N, B, R, Q, K
int piece_values[] = { 0, -100, -350, -350, -525, -1000, -10000, 100, 350, 350, 525, 1000, 10000 };

int pawn_table[] = {
      0,  0,  0,  0,  0,  0,  0,  0,
     50, 50, 50, 50, 50, 50, 50, 50,
     10, 10, 20, 30, 30, 20, 10, 10,
      5,  5, 10, 25, 25, 10,  5,  5,
      0,  0,  0, 20, 20,  0,  0,  0,
      5, -5,-10,  0,  0,-10, -5,  5,
      5, 10, 10,-20,-20, 10, 10,  5,
      0,  0,  0,  0,  0,  0,  0,  0,
};

int knight_table[] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

int bishop_table[] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

int rook_table[] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      5, 10, 10, 10, 10, 10, 10,  5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
      0,  0,  0,  5,  5,  0,  0,  0,
};

int queen_table[] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20,
};

int king_table[] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  5,  5,  5,  5,  0,  0,
      0,  5,  5, 10, 10,  5,  5,  0,
      0,  5, 10, 20, 20, 10,  5,  0,
      0,  5, 10, 20, 20, 10,  5,  0,
      0,  0,  5, 10, 10,  5,  0,  0,
      0,  5,  5, -5, -5,  0,  5,  0,
      0,  0,  5,  0,-15,  0,  10, 0,
};

int mirror_square[] = {
    56, 57, 58, 59, 60, 61, 62, 63,
    48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47,
    32, 33, 34, 35, 36, 37, 38, 39,
    24, 25, 26, 27, 28, 29, 30, 31,
    16, 17, 18, 19, 20, 21, 22, 23,
     8,  9, 10, 11, 12, 13, 14, 15,
     0,  1,  2,  3,  4,  5,  6,  7,
};

inline int get_square_in_64(int square) {
    return (square >> 4) * 8 + (square & 7);
}

int evaluate_white_pawn_structure(Position pos, int square) {
    bool is_passed = true;
    bool is_isolated = true;


    while (square > h7) {
        if (pos.board[square - 15] == p ||
            pos.board[square - 16] == p ||
            pos.board[square - 16] == P ||
            pos.board[square - 17] == p) {
            is_passed = false;
            break;
        }
        square -= 16;
    }
    
    int file = square & 7;
    square = file + 16;
    while (square < a1) {
        if (pos.board[square - 1] == P || pos.board[square + 1] == P) {
            is_isolated = false;
            break;
        }
        square += 16;
    }

    // cout << "passed: " << is_passed << endl;
    // cout << "isolated: " << is_isolated << endl;
    return (is_passed * 50) - (is_isolated * 10);
}

int evaluate_black_pawn_structure(Position pos, int square) {
    bool is_passed = true;
    bool is_isolated = true;

    while (square < a2) {
        if (pos.board[square + 15] == P || 
            pos.board[square + 16] == P || 
            pos.board[square + 16] == p || 
            pos.board[square + 17] == P) {
            is_passed = false;
            break;
        }
        square += 16;
    }
    
    int file = square & 7;
    square = file + 16;
    while (square < a1) {
        if (pos.board[square - 1] == p || pos.board[square + 1] == p) {
            is_isolated = false;
            break;
        }
        square += 16;
    }

    // cout << "passed: " << is_passed << endl;
    // cout << "isolated: " << is_isolated << endl;
    return (is_passed * 50) - (is_isolated * 10);
    
}

/* pawn_rank[x][y] is the rank of the least advanced pawn of color x on file
   y - 1. There are "buffer files" on the left and right to avoid special-case
   logic later. If there's no pawn on a rank, we pretend the pawn is
   impossibly far advanced (0 for LIGHT and 7 for DARK). This makes it easy to
   test for pawns on a rank and it simplifies some pawn evaluation code. */
int pawn_rank[2][10];

#define DOUBLED_PAWN_PENALTY		10
#define ISOLATED_PAWN_PENALTY		20
#define BACKWARDS_PAWN_PENALTY		8
#define PASSED_PAWN_BONUS			20
#define ROOK_SEMI_OPEN_FILE_BONUS	10
#define ROOK_OPEN_FILE_BONUS		15
#define ROOK_ON_SEVENTH_BONUS		20


int eval_white_pawn(int square, int rank, int file) {
    int score = 0;

    file += 1;

    /* if there's a pawn behind this one, it's doubled */
	if (pawn_rank[white][file] > rank)
        score -= DOUBLED_PAWN_PENALTY;

	/* if there aren't any friendly pawns on either side of
	   this one, it's isolated */
	if ((pawn_rank[white][file - 1] == 0) && (pawn_rank[white][file + 1] == 0))
        score -= ISOLATED_PAWN_PENALTY;

	/* if it's not isolated, it might be backwards */
	else if ((pawn_rank[white][file - 1] < rank) && (pawn_rank[white][file + 1] < rank))
		score -= BACKWARDS_PAWN_PENALTY;

	/* add a bonus if the pawn is passed */
	if ((pawn_rank[black][file - 1] >= rank) &&
		    (pawn_rank[black][file] >= rank) &&
		    (pawn_rank[black][file + 1] >= rank))
		score += (7 - rank) * PASSED_PAWN_BONUS;

	return score;
}

int eval_black_pawn(int square, int rank, int file) {
    int score = 0;

    file += 1;

    /* if there's a pawn behind this one, it's doubled */
	if (pawn_rank[black][file] < rank)
		score -= DOUBLED_PAWN_PENALTY;

	/* if there aren't any friendly pawns on either side of
	   this one, it's isolated */
	if ((pawn_rank[black][file - 1] == 7) && (pawn_rank[black][file + 1] == 7))
		score -= ISOLATED_PAWN_PENALTY;

	/* if it's not isolated, it might be backwards */
	else if ((pawn_rank[black][file - 1] > rank) && (pawn_rank[black][file + 1] > rank))
		score -= BACKWARDS_PAWN_PENALTY;

	/* add a bonus if the pawn is passed */
	if ((pawn_rank[white][file - 1] <= rank) &&
			(pawn_rank[white][file] <= rank) &&
			(pawn_rank[white][file + 1] <= rank))
		score += rank * PASSED_PAWN_BONUS;

	return score;
}


int evaluate_position(Position &pos) {
	int file, rank;
	int score = 0;
    int square;
    int piece;

	/* this is the first pass: set up pawn_rank */
	for (int i = 0; i < 10; i++) {
		pawn_rank[white][i] = 0;
		pawn_rank[black][i] = 7;
	}

    for (rank = 0; rank < 8; rank++) {
        for (file = 0; file < 8; file++) {
            square = rank * 16 + file;
            piece = pos.board[square];
            if (piece == e)
                continue;

            if (piece == P && pawn_rank[white][file + 1] < rank) 
                pawn_rank[white][file + 1] = rank;

            else if (piece == p && pawn_rank[black][file + 1] > rank)
                pawn_rank[black][file + 1] = rank;
        }
	}

    /* this is the second pass: evaluate each piece */

    for (rank = 0; rank < 8; rank++) {
        for (file = 0; file < 8; file++) {
            // square as 0..127
            square = rank * 16 + file;

            if (pos.board[square] != e) {
                piece = pos.board[square];
                score += piece_values[piece];
                switch (piece) {
                case P:
                    score += pawn_table[get_square_in_64(square)];
                    score += eval_white_pawn(square, rank, file);
                    //cout << score << endl;
                    break;
                case N:
                    score += knight_table[get_square_in_64(square)];
                    break;
                case B:
                    score += bishop_table[get_square_in_64(square)];
                    break;
                case R:
                    score += rook_table[get_square_in_64(square)];
                    break;
                case Q:
                    score += queen_table[get_square_in_64(square)];
                    break;
                case K:
                    score += king_table[get_square_in_64(square)];
                    break;
                case p:
                    score -= pawn_table[mirror_square[get_square_in_64(square)]];
                    score -= eval_black_pawn(square, rank, file);
                    //cout << score << endl;
                    break;
                case n:
                    score -= knight_table[mirror_square[get_square_in_64(square)]];
                    break;
                case b:
                    score -= bishop_table[mirror_square[get_square_in_64(square)]];
                    break;
                case r:
                    score -= rook_table[mirror_square[get_square_in_64(square)]];
                    break;
                case q:
                    score -= queen_table[mirror_square[get_square_in_64(square)]];
                    break;
                case k:
                    score -= king_table[mirror_square[get_square_in_64(square)]];
                    break;
                
                default:
                    break;
                }
            }
        }
    }
    return pos.side == white ? score : -score;
}


int evaluate_position2(Position &pos) {
    int score = 0;
    int square, square_in_64, piece;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            // square as 0..127
            square = rank * 16 + file;
            //square_in_64 = rank * 8 + file;
            piece = pos.board[square];
            //cout << square_in_64 << endl;
            if (piece != e) {
                square_in_64 = get_square_in_64(square);
                score += piece_values[piece];
                
                switch (piece) {
                case P:
                    score += pawn_table[square_in_64];
                    //score += evaluate_white_pawn_structure(pos, square);
                    //cout << score << endl;
                    break;
                case N:
                    score += knight_table[square_in_64];
                    break;
                case B:
                    score += bishop_table[square_in_64];
                    break;
                case R:
                    score += rook_table[square_in_64];
                    break;
                case Q:
                    score += queen_table[square_in_64];
                    break;
                case K:
                    score += king_table[square_in_64];
                    break;
                case p:
                    score -= pawn_table[mirror_square[square_in_64]];
                    //score -= evaluate_black_pawn_structure(pos, square);
                    //cout << score << endl;
                    break;
                case n:
                    score -= knight_table[mirror_square[square_in_64]];
                    break;
                case b:
                    score -= bishop_table[mirror_square[square_in_64]];
                    break;
                case r:
                    score -= rook_table[mirror_square[square_in_64]];
                    break;
                case q:
                    score -= queen_table[mirror_square[square_in_64]];
                    break;
                case k:
                    score -= king_table[mirror_square[square_in_64]];
                    break;
                
                default:
                    break;
                }
            }
        }
    }
    // assert(pos.side == white ? pos.material_score == score : -pos.material_score == -score);
    
    return pos.side == white ? score : -score;
    //return pos.side == white ? pos.material_score : -pos.material_score;
}
