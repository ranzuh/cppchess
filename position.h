#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <map>

using namespace std;

// constants

// encode pieces
enum pieces {
    e, p, n, b, r, q, k, P, N, B, R, Q, K, o
};

// encode squares
enum squares : int {
    a8 = 0,   b8, c8, d8, e8, f8, g8, h8, 
    a7 = 16,  b7, c7, d7, e7, f7, g7, h7, 
    a6 = 32,  b6, c6, d6, e6, f6, g6, h6,
    a5 = 48,  b5, c5, d5, e5, f5, g5, h5,
    a4 = 64,  b4, c4, d4, e4, f4, g4, h4,
    a3 = 80,  b3, c3, d3, e3, f3, g3, h3,
    a2 = 96,  b2, c2, d2, e2, f2, g2, h2,
    a1 = 112, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// castling binary representation
//
//  bin  dec
// 0001    1  white king can castle to the king side
// 0010    2  white king can castle to the queen side
// 0100    4  black king can castle to the king side
// 1000    8  black king can castle to the queen side
//
// examples
// 1111       both sides an castle both directions
// 1001       black king => queen side
//            white king => king side
// 1111 & 0001 = 1 (not 0)
// 0000 & 0001 = 0 
// if (castle & Kc)... for example
enum castling : int { Kc = 1, Qc = 2, kc = 4, qc = 8};

enum sides : int { white, black };

struct Position {
    static string square_to_coord[128];

    static string ascii_pieces;

    static string unicode_pieces[13];

    // status
    int board[128] = {
        r, n, b, q, k, b, n, r,   o, o, o, o, o, o, o, o,
        p, p, p, p, p, p, p, p,   o, o, o, o, o, o, o, o,
        e, e, e, e, e, e, e, e,   o, o, o, o, o, o, o, o,
        e, e, e, e, e, e, e, e,   o, o, o, o, o, o, o, o,
        e, e, e, e, e, e, e, e,   o, o, o, o, o, o, o, o,
        e, e, e, e, e, e, e, e,   o, o, o, o, o, o, o, o,
        P, P, P, P, P, P, P, P,   o, o, o, o, o, o, o, o,
        R, N, B, Q, K, B, N, R,   o, o, o, o, o, o, o, o
    };

    // side to move
    int side = white;

    // enpassant square
    int enpassant = no_sq;

    // castling rights dec 15 = bin 1111 all can castle
    int castle = 15;

    // both king's squares
    int king_squares[2] = { e1, e8 };

    int init_depth;

    // following defined in board.cpp
    void print_board();
    void reset_board();
    void parse_fen(string fen);
    void print_board_stats();
    int make_move(int move);
    uint64_t perft(int depth);
};

#endif