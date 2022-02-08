#include <iostream>
#include <string>
#include "position.h"
#include "movegen.h"
#include "hashtable.h"
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

inline int get_square_in_64(int square) {
    return (square >> 4) * 8 + (square & 7);
}

void Position::unmake_move(int move, int depth) {

    // change side
    // do this first to make rest easier
    side = !side;
    hash_key ^= side_key;

    // decode move
    int from_square = decode_source(move);
    int to_square = decode_target(move);
    int promoted_piece = decode_promotion(move);
    int is_capture = decode_capture(move);
    int is_enpassant = decode_enpassant(move);
    int is_double_pawn = decode_double_pawn(move);
    int is_castling = decode_castling(move);

    // source piece we know - its in to square
    // target piece we dont know
    // store piece
    int source_piece = board[to_square];
    int target_piece = previous_piece[depth];

    // make move
    board[from_square] = board[to_square];
    board[to_square] = previous_piece[depth];  // todo restore previous piece

    // add same piece to next square if not promotion // reverse should just work
    if (!promoted_piece) {
        // remove piece from previous square // reverse should just work
        hash_key ^= piece_keys[source_piece][get_square_in_64(from_square)];
        hash_key ^= piece_keys[source_piece][get_square_in_64(to_square)];
    }

    // update king square
    if (board[from_square] == K || board[from_square] == k) {
        king_squares[side] = from_square;
    }

    // reverse should just work
    if (is_capture && !is_enpassant) {
        hash_key ^= piece_keys[target_piece][get_square_in_64(to_square)];
    }

    // Need to store previous enpassant
    // hash enpassant if available (remove enpassant square from hash key )
    if (previous_enpassant[depth] != no_sq) hash_key ^= enpassant_keys[get_square_in_64(previous_enpassant[depth])];

    // reset enpassant square
    enpassant = previous_enpassant[depth];

    // handle promotion
    if (promoted_piece != e) {
        // if promotion set pawn to previous square
        side == white ? board[from_square] = P : board[from_square] = p;

        // remove piece from previous square // reverse should just work
        hash_key ^= piece_keys[board[from_square]][get_square_in_64(from_square)];

        // add promoted piece to square // reverse should just work
        hash_key ^= piece_keys[promoted_piece][get_square_in_64(to_square)];
    }
    else if (is_enpassant) {
        side == white ? board[to_square + 16] = p : board[to_square - 16] = P;
        // hash enpassant
        side == white ? hash_key ^= piece_keys[p][get_square_in_64(to_square + 16)] : hash_key ^= piece_keys[P][get_square_in_64(to_square - 16)];
    }
    else if (is_double_pawn) {
        side == white ? enpassant = previous_enpassant[depth] : enpassant = previous_enpassant[depth];
        // hashing
        side == white ? hash_key ^= enpassant_keys[get_square_in_64(to_square + 16)] : hash_key ^= enpassant_keys[get_square_in_64(to_square - 16)];
    }
    else if (is_castling) {
        switch (to_square) {
            // white queen side castling
            case c1:
                board[d1] = e;
                board[a1] = R;
                hash_key ^= piece_keys[R][get_square_in_64(a1)];
                hash_key ^= piece_keys[R][get_square_in_64(d1)];
                break;
            // white king side castling
            case g1:
                board[f1] = e;
                board[h1] = R;
                hash_key ^= piece_keys[R][get_square_in_64(h1)];
                hash_key ^= piece_keys[R][get_square_in_64(f1)];
                break;
            // black queen side castling
            case c8:
                board[d8] = e;
                board[a8] = r;
                hash_key ^= piece_keys[r][get_square_in_64(a8)];
                hash_key ^= piece_keys[r][get_square_in_64(d8)];
                break;
            // black king    side castling
            case g8:
                board[f8] = e;
                board[h8] = r;
                hash_key ^= piece_keys[r][get_square_in_64(h8)];
                hash_key ^= piece_keys[r][get_square_in_64(f8)];
                break;
            default:
                break;
        }
    }
    // remove old castle from hash
    hash_key ^= castle_keys[castle];

    castle = previous_castle[depth];

    // add new castle to hash
    hash_key ^= castle_keys[castle];

    enpassant = previous_enpassant[depth];
    castle = previous_castle[depth];

}

// make a move on the board if its legal
// returns 1 if legal, 0 if illegal
// todo differentiate between captures and all_moves
int Position::make_move(int move, int depth) {

    // copy board state
    // copy(board, board + 128, board_copy);
    // copy(king_squares, king_squares + 2, king_squares_copy);
    // side_copy = side;
    // enpassant_copy = enpassant;
    // castle_copy = castle;
    // uint64_t hash_copy = hash_key;

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

    // store position variables for unmake
    previous_enpassant[depth] = enpassant;
    previous_castle[depth] = castle;
    previous_piece[depth] = target_piece;

    // make move
    board[to_square] = board[from_square];
    board[from_square] = e;

    // remove piece from previous square
    hash_key ^= piece_keys[source_piece][get_square_in_64(from_square)];
    // add same piece to next square if not promotion
    if (!promoted_piece) {
        hash_key ^= piece_keys[source_piece][get_square_in_64(to_square)];
    }

    // update king square
    if (board[to_square] == K || board[to_square] == k) {
        king_squares[side] = to_square;
    }

    if (is_capture && !is_enpassant) {
        hash_key ^= piece_keys[target_piece][get_square_in_64(to_square)];
    }

    // hash enpassant if available (remove enpassant square from hash key )
    if (enpassant != no_sq) hash_key ^= enpassant_keys[get_square_in_64(enpassant)];

    // reset enpassant square
    enpassant = no_sq;

    // handle promotion
    if (promoted_piece != e) {
        board[to_square] = promoted_piece;

        // add promoted piece to square
        hash_key ^= piece_keys[promoted_piece][get_square_in_64(to_square)];
    }
    else if (is_enpassant) {
        side == white ? board[to_square + 16] = e : board[to_square - 16] = e;
        // hash enpassant
        side == white ? hash_key ^= piece_keys[p][get_square_in_64(to_square + 16)] : hash_key ^= piece_keys[P][get_square_in_64(to_square - 16)];
    }
    else if (is_double_pawn) {
        side == white ? enpassant = to_square + 16 : enpassant = to_square - 16;
        // hashing
        side == white ? hash_key ^= enpassant_keys[get_square_in_64(to_square + 16)] : hash_key ^= enpassant_keys[get_square_in_64(to_square - 16)];
    }
    else if (is_castling) {
        switch (to_square) {
            // white queen side castling
            case c1:
                board[d1] = board[a1];
                board[a1] = e;
                hash_key ^= piece_keys[R][get_square_in_64(a1)];
                hash_key ^= piece_keys[R][get_square_in_64(d1)];
                break;
            // white king side castling
            case g1:
                board[f1] = board[h1];
                board[h1] = e;
                hash_key ^= piece_keys[R][get_square_in_64(h1)];
                hash_key ^= piece_keys[R][get_square_in_64(f1)];
                break;
            // black queen side castling
            case c8:
                board[d8] = board[a8];
                board[a8] = e;
                hash_key ^= piece_keys[r][get_square_in_64(a8)];
                hash_key ^= piece_keys[r][get_square_in_64(d8)];
                break;
            // black king    side castling
            case g8:
                board[f8] = board[h8];
                board[h8] = e;
                hash_key ^= piece_keys[r][get_square_in_64(h8)];
                hash_key ^= piece_keys[r][get_square_in_64(f8)];
                break;
            default:
                break;
        }
    }

    // unmake move if king under check
    bool in_check = is_square_attacked(*this, king_squares[side], !side);
        // restore board state
        // copy(board_copy, board_copy + 128, board);
        // copy(king_squares_copy, king_squares_copy + 2, king_squares);
        // side = side_copy;
        // enpassant = enpassant_copy;
        // castle = castle_copy;
        // hash_key = hash_copy;

        // illegal move
        //return 0;
    //}

    // remove old castle from hash
    hash_key ^= castle_keys[castle];

    // update castling rights
    // king or rook has moved
    castle &= castling_board[from_square];
    // rook was captured
    castle &= castling_board[to_square];

    // add new castle to hash
    hash_key ^= castle_keys[castle];

    // change side
    side = !side;
    hash_key ^= side_key;

    // illegal move
    if (in_check) return 0;
    // legal move
    else return 1;
    
}
