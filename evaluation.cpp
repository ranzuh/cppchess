#include "position.h"
//#include "evaluation.h"

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


int evaluate_position(Position &pos) {
    int score = 0;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            // square as 0..127
            int square = rank * 16 + file;
            int square_in_64 = rank * 8 + file;
            //cout << square_in_64 << endl;
            if (!(square & 0x88) && pos.board[square] != e) {
                int piece = pos.board[square];
                score += piece_values[piece];
                
                switch (piece) {
                case P:
                    score += pawn_table[square_in_64];
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
