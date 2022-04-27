#ifndef MOVEGEN_H
#define MOVEGEN_H

struct Movelist
{
    int moves[256];
    int count;

    Movelist() {
        count = 0;
    }

    inline void add_move(int move) {
        // add move to the move list
        moves[count] = move;
        // increment move count
        count++;
    }

    inline void reset() {
        count = 0;
    }
};

/* 
Move encoding

0000 0000 0000 0000 0111 1111   source square       7 bits  0x7f
0000 0000 0011 1111 1000 0000   target square       7 bits  0x7f
0000 0011 1100 0000 0000 0000   promotion piece     4 bits  0xf
0000 0100 0000 0000 0000 0000   capture flag        1 bit   0x1
0000 1000 0000 0000 0000 0000   double pawn flag    1 bit   0x1
0001 0000 0000 0000 0000 0000   en passant flag     1 bit   0x1
0010 0000 0000 0000 0000 0000   castling flag       1 bit   0x1

*/
inline int encode_move(
    int source, 
    int target, 
    int promotion,
    int capture,
    int double_pawn, 
    int enpassant, 
    int castling) 
{
    return 
        source | 
        target << 7 | 
        promotion << 14 | 
        capture << 18 | 
        double_pawn << 19 | 
        enpassant << 20 | 
        castling << 21;
}

/* Decoding move */

inline int decode_source(int move) {
    return move & 0x7f;
}

inline int decode_target(int move) {
    return (move >> 7) & 0x7f; 
}

inline int decode_promotion(int move) {
    return (move >> 14) & 0xf;
}

inline int decode_capture(int move) {
    return (move >> 18) & 0x1;
}

inline int decode_double_pawn(int move) {
    return (move >> 19) & 0x1;
}

inline int decode_enpassant(int move) {
    return (move >> 20) & 0x1;
}

inline int decode_castling(int move) {
    return (move >> 21) & 0x1;
}

/* Forward declarations */

int generate_pseudo_moves(Position &pos, Movelist &moves);
int generate_pseudo_tactical(Position &pos, Movelist &moves);
int generate_legal_moves(Position &pos, Movelist &moves);
void print_movelist(Movelist &moves);
int is_square_attacked(Position &pos, int square, int side);
string get_move_string(int move);
string get_san_string(Position &pos, int move);

#endif