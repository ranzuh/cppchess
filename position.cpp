#include <iostream>
#include <string>
#include "position.h"
#include "movegen.h"
#include <chrono>

using namespace std;

string Position::square_to_coord[128] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "i8", "j8", "k8", "l8", "m8", "n8", "o8", "p8", 
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "i7", "j7", "k7", "l7", "m7", "n7", "o7", "p7", 
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "i6", "j6", "k6", "l6", "m6", "n6", "o6", "p6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "i5", "j5", "k5", "l5", "m5", "n5", "o5", "p5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "i4", "j4", "k4", "l4", "m4", "n4", "o4", "p4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "i3", "j3", "k3", "l3", "m3", "n3", "o3", "p3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "i2", "j2", "k2", "l2", "m2", "n2", "o2", "p2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "i1", "j1", "k1", "l1", "m1", "n1", "o1", "p1"
};

string Position::ascii_pieces = ".pnbrqkPNBRQKo";

string Position::unicode_pieces[13] = {
    ".", "♟", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"
};

/*
                            castle & map    bin     dec
white king moved            1111 & 1100 =   1100    12
white kings rook moved      1111 & 1110 =   1110    14
white queens rook moved     1111 & 1101 =   1101    13

black king moved            1111 & 0011 =   0011    3
black kings rook moved      1111 & 1011 =   1011    11
black queens rook moved     1111 & 0111 =   0111    7

*/
int castling_board[128] {
    7,  15, 15, 15, 3,  15, 15, 11,   o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,   o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,   o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,   o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,   o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,   o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,   o, o, o, o, o, o, o, o,
    13, 15, 15, 15, 12, 15, 15, 14,   o, o, o, o, o, o, o, o, 
};

// convert ascii pieces to int
map<char, int> char_pieces = {
    {'p', p}, 
    {'n', n}, 
    {'b', b},
    {'r', r},
    {'q', q},
    {'k', k},
    {'P', P},
    {'N', N},
    {'B', B},
    {'R', R},
    {'Q', Q},
    {'K', K}
};

// print the chess board
void Position::print_board() {
    // new line
    cout << endl;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 16; file++) {
            // square as 0..127
            int square = rank * 16 + file;

            // print rank numbers
            if (file == 0) {
                cout << 8 - rank << "   ";
            }

            // print square as ascii if not outside
            if (!(square & 0x88)) {
                cout << ascii_pieces[board[square]] << ' ';
            }
        }

        // add new line after each rank
        cout << endl;
    }

    // print file letters
    cout << endl << "    a b c d e f g h" << endl << endl;;
}

// empties the board
void Position::reset_board() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 16; file++) {
            // square as 0..127
            int square = rank * 16 + file;

            // set square as empty if not outside
            if (!(square & 0x88)) {
                board[square] = e;
            }
        }
    }
}

void Position::set_material() {
    int score = 0;
    for (int square = 0; square < 128; square++) {
        if (!(square & 0x88)) {
            score += piece_values[board[square]];
            //cout << pos.board[square] << " " << piece_values[pos.board[square]] << endl;
        }
    }
    material_score = score;
}

// generate "almost unique" position ID aka hash key from scratch
uint64_t generate_hash_key(Position &pos);

// parses fen string and sets it to board
void Position::parse_fen(string fen) {
    reset_board();
    int i = 0;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 16; file++) {
            // square as 0..127
            int square = rank * 16 + file;

            if (!(square & 0x88)) {
                // match characters
                if ((fen[i] > 'a' && fen[i] < 'z') || (fen[i] > 'A' && fen[i] < 'Z')) {
                    
                    // set king's positions
                    if (fen[i] == 'K') {
                        king_squares[white] = square;
                    }
                    else if (fen[i] == 'k') {
                        king_squares[black] = square;
                    }

                    // set piece
                    board[square] = char_pieces[fen[i]];

                    // increment index
                    i++;
                }

                // match empty squares
                if (fen[i] >= '0' && fen[i] <= '9') {
                    // calculate offset (convert char to int)
                    int offset = fen[i] - '0';

                    // decrement file on empty square
                    if (!board[square]) {
                        file--;
                    }

                    // skip empty squares
                    file += offset;

                    // increment index
                    i++;
                }

                // match end of rank
                if (fen[i] == '/') {
                    // increment index
                    i++;
                }
            }
        }
    }

    // increment index
    i++;

    // parse side to move
    side = fen[i] == 'w' ? white : black;

    // increment index twice to goto castling rights
    i += 2;

    // parse castling
    castle = 0;
    while (fen[i] != ' ') {
        switch (fen[i]) {
            case 'K': 
                castle |= Kc;
                break;
            case 'Q': 
                castle |= Qc;
                break;
            case 'k': 
                castle |= kc;
                break;
            case 'q': 
                castle |= qc;
                break;
            default:
                break;
        }

        // increment index
        i++;
    }

    // goto en passant square
    i++;

    // parse en passant
    if (fen[i] != '-') {
        int file = fen[i] - 'a';
        int rank = fen[i+1] - '0';
        enpassant = (8 - rank) * 16 + file;
    } else {
        enpassant = no_sq;
    }

    set_material();

    // set hash key
    hash_key = generate_hash_key(*this);
}

// print board status infos
void Position::print_board_stats() {
    cout << "    Side to move:    " << (side == white ? "white" : "black") << endl;
    cout << "    Castling rights: ";
    cout << (castle & Kc ? 'K' : '-');
    cout << (castle & Qc ? 'Q' : '-');
    cout << (castle & kc ? 'k' : '-');
    cout << (castle & qc ? 'q' : '-') << endl;
    cout << "    Enpassant:       " << (enpassant != no_sq ? square_to_coord[enpassant] : "-") << endl;
    cout << "    Kings square:    " << square_to_coord[king_squares[side]] << endl;
    cout << "    Material:        " << material_score << endl;
    cout << "    Hash key:        " << hex << hash_key << dec << endl << endl;
}

// make a move on the board if its legal
// returns 1 if legal, 0 if illegal
// todo differentiate between captures and all_moves
int Position::make_move(int move) {
    // define board state copies
    // int board_copy[128], king_squares_copy[2], side_copy, enpassant_copy, castle_copy;

    // copy board state
    // copy(board, board + 128, board_copy);
    // copy(king_squares, king_squares + 2, king_squares_copy);
    // side_copy = side;
    // enpassant_copy = enpassant;
    // castle_copy = castle;

    // decode move
    int from_square = decode_source(move);
    int to_square = decode_target(move);
    int promoted_piece = decode_promotion(move);
    int is_capture = decode_capture(move);
    int is_enpassant = decode_enpassant(move);
    int is_double_pawn = decode_double_pawn(move);
    int is_castling = decode_castling(move);

    // store piece
    int source_piece = board[from_square];
    int target_piece = board[to_square];

    // make move
    board[to_square] = board[from_square];
    board[from_square] = e;

    // update king square
    if (board[to_square] == K || board[to_square] == k) {
        king_squares[side] = to_square;
    }

    if (is_capture) {
        material_score -= piece_values[target_piece];
    }

    // unmake move if king under check
    // if (is_square_attacked(king_squares[side], !side)) {
    //     // restore board state
    //     copy(board_copy, board_copy + 128, board);
    //     copy(king_squares_copy, king_squares_copy + 2, king_squares);
    //     side = side_copy;
    //     enpassant = enpassant_copy;
    //     castle = castle_copy;

    //     // illegal move
    //     return 0;
    // }

    // reset enpassant square
    enpassant = no_sq;

    // handle promotion
    if (promoted_piece != e) {
        board[to_square] = promoted_piece;
        //board[from_square] = e;
        material_score += piece_values[promoted_piece];
        material_score -= piece_values[source_piece];
    }
    else if (is_enpassant) {
        //board[to_square] = board[from_square];
        //board[from_square] = e;
        side == white ? material_score -= piece_values[board[to_square + 16]] : material_score -= piece_values[board[to_square - 16]];
        side == white ? board[to_square + 16] = e : board[to_square - 16] = e;
    }
    else if (is_double_pawn) {
        // board[to_square] = board[from_square];
        // board[from_square] = e;
        side == white ? enpassant = to_square + 16 : enpassant = to_square - 16;
    }
    else if (is_castling) {
        // board[to_square] = board[from_square];
        // board[from_square] = e;
        switch (to_square) {
            // white queen side castling
            case c1:
                board[d1] = board[a1];
                board[a1] = e;
                break;
            // white king side castling
            case g1:
                board[f1] = board[h1];
                board[h1] = e;
                break;
            // black queen side castling
            case c8:
                board[d8] = board[a8];
                board[a8] = e;
                break;
            // black king    side castling
            case g8:
                board[f8] = board[h8];
                board[h8] = e;
                break;
            default:
                break;
        }
    }
    else {
        // make move
        // board[to_square] = board[from_square];
        // board[from_square] = e;
    }

    // update castling rights
    // king or rook has moved
    castle &= castling_board[from_square];
    // rook was captured
    castle &= castling_board[to_square];

    // change side
    side = !side;

    //
    // ====== debug hash key incremental update ======= //
    //
    
    // build hash key for the updated position (after move is made) from scratch Nodes per second: 18267536
    hash_key = generate_hash_key(*this);

    // increment repetition index & store hash key
    rep_index += 1;
    rep_stack[rep_index] = hash_key;
    
    // in case if hash key built from scratch doesn't match
    // the one that was incrementally updated we interrupt execution
    // if (hash_key != hash_from_scratch)
    // {
    //     cout << "Make move" << endl;
    //     cout << "move: " << get_move_string2(move) << endl;
    //     print_board();
    //     print_board_stats();
    //     cout << "hash key should be: " << hex << hash_from_scratch << dec << endl;
    //     string mystring;
    //     cin >> mystring;
    // }

    // legal move
    return 1;
}
