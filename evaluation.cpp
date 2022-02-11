#include "position.h"

////////////////
// evaluation //
////////////////

// e, p, n, b, r, q, k, P, N, B, R, Q, K
int piece_values[] = { 0, -100, -320, -330, -500, -900, -10000, 100, 320, 330, 500, 900, 10000 };

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

// map square in 0..127 -> 0..63
inline int get_square_in_64(int square) {
    return (square >> 4) * 8 + (square & 7);
}

// store least advanced pawn on each file for each side
// 0 and 9 files are used as a buffer to help with isolated 
// and backward pawn calculations.
int pawn_rank[2][10];

// scores for pawn structures
#define doubled_pawn_penalty        10
#define isolated_pawn_penalty       20
#define backwards_pawn_penalty      8
#define passed_pawn_bonus           20
#define rook_semi_open_file_bonus   10
#define rook_open_file_bonus        15
#define rook_on_seventh_bonus       20

// evaluate white pawn structures
int eval_white_pawn(int square, int rank, int file) {
    int score = 0;

    // increment file (remember I used 0..9 files)
    file += 1;

    // if this pawn is not the least advanced one, its doubled
	if (pawn_rank[white][file] > rank)
        score -= doubled_pawn_penalty;

	// if no friendly pawns on adjacent files its isolated
	if ((pawn_rank[white][file - 1] == 0) && (pawn_rank[white][file + 1] == 0))
        score -= isolated_pawn_penalty;

	// if pawn not isolated it might be backwards
	else if ((pawn_rank[white][file - 1] < rank) && (pawn_rank[white][file + 1] < rank))
		score -= backwards_pawn_penalty;

	// give bonus if pawn is passed
	if ((pawn_rank[black][file - 1] >= rank) &&
		    (pawn_rank[black][file] >= rank) &&
		    (pawn_rank[black][file + 1] >= rank))
		score += (7 - rank) * passed_pawn_bonus;

	return score;
}

// evaluate white pawn structures
int eval_black_pawn(int square, int rank, int file) {
    int score = 0;

    // increment file (remember I used 0..9 files)
    file += 1;

    // if this pawn is not the least advanced one, its doubled
	if (pawn_rank[black][file] < rank)
		score -= doubled_pawn_penalty;

	// if no friendly pawns on adjacent files its isolated
	if ((pawn_rank[black][file - 1] == 7) && (pawn_rank[black][file + 1] == 7))
		score -= isolated_pawn_penalty;

	// if pawn not isolated it might be backwards
	else if ((pawn_rank[black][file - 1] > rank) && (pawn_rank[black][file + 1] > rank))
		score -= backwards_pawn_penalty;

	// give bonus if pawn is passed
	if ((pawn_rank[white][file - 1] <= rank) &&
			(pawn_rank[white][file] <= rank) &&
			(pawn_rank[white][file + 1] <= rank))
		score += rank * passed_pawn_bonus;

	return score;
}

// evaluate whites rook
int evaluate_white_rook(int rank, int file) {
    int score = 0;

    // check if there is black pawn on same file as the rook
    if (pawn_rank[black][file + 1] == 7) {
        // check if there is also white pawn on same file as the rook
        if (pawn_rank[white][file + 1] == 0)
            score += rook_open_file_bonus;
        else
            score += rook_semi_open_file_bonus;
    }

    // check if rook is on seventh rank
    if (rank == 1)
        score += rook_on_seventh_bonus;
    
    return score;
}

// evaluate blacks rook
int evaluate_black_rook(int rank, int file) {
    int score = 0;

    // check if there is white pawn on same file as the rook
    if (pawn_rank[white][file + 1] == 0) {
        // check if there is also black pawn on same file as the rook
        if (pawn_rank[black][file + 1] == 7)
            score += rook_open_file_bonus;
        else
            score += rook_semi_open_file_bonus;
    }

    // check if rook is on second rank
    if (rank == 6)
        score += rook_on_seventh_bonus;
    
    return score;
}

/* evaluate position based on material, piece-square tables and
   basic pawn structures */
int evaluate_position(Position &pos) {
	int file, rank;
	int score = 0;
    int square;
    int piece;

	/* first store least advanced pawns on every file */

    // init each file to max advancement for each side
	for (int i = 0; i < 10; i++) {
		pawn_rank[white][i] = 0;
		pawn_rank[black][i] = 7;
	}

    // store the pawns
    for (rank = 1; rank < 7; rank++) {
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

    /* evaluate each pawn or piece */

    for (rank = 0; rank < 8; rank++) {
        for (file = 0; file < 8; file++) {
            // square as 0..127
            square = rank * 16 + file;

            if (pos.board[square] != e) {
                piece = pos.board[square];
                // sum the material
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
                    score += evaluate_white_rook(rank, file);
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
                    score -= evaluate_black_rook(rank, file);
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
