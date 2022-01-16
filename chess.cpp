// based on tutorial by Chess Programming @ youtube (https://www.youtube.com/playlist?list=PLmN0neTso3JzhJP35hwPHJi4FZgw5Ior0)

#include <iostream>
#include <string>
#include <map>

using namespace std;

string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
string tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

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
enum castling { Kc = 1, Qc = 2, kc = 4, qc = 8};

enum sides { white, black };

string ascii_pieces = ".pnbrqkPNBRQKo";

string unicode_pieces[] = {
    ".", "♟", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"
};

int board[128] = {
    r, n, b, q, k, b, n, r, o, o, o, o, o, o, o, o,
    p, p, p, p, p, p, p, p, o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e, o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e, o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e, o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e, o, o, o, o, o, o, o, o,
    P, P, P, P, P, P, P, P, o, o, o, o, o, o, o, o,
    R, N, B, Q, K, B, N, R, o, o, o, o, o, o, o, o
};

// side to move
int side = white;

// enpassant square
int enpassant = no_sq;

// castling rights dec 15 = bin 1111 all can castle
int castle = 15;

string square_to_coord[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "i8", "j8", "k8", "l8", "m8", "n8", "o8", "p8", 
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "i7", "j7", "k7", "l7", "m7", "n7", "o7", "p7", 
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "i6", "j6", "k6", "l6", "m6", "n6", "o6", "p6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "i5", "j5", "k5", "l5", "m5", "n5", "o5", "p5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "i4", "j4", "k4", "l4", "m4", "n4", "o4", "p4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "i3", "j3", "k3", "l3", "m3", "n3", "o3", "p3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "i2", "j2", "k2", "l2", "m2", "n2", "o2", "p2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "i1", "j1", "k1", "l1", "m1", "n1", "o1", "p1"
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

// piece move offsets

int knight_offsets[] = { -33, -31, -18, -14,  14, 18, 31, 33 };
int bishop_offsets[] = { -17, -15, 15, 17 };
int rook_offsets[] = { -16, -1, 1, 16 };
int king_offsets[] = { -17, -16, -15, -1, 1, 15, 16, 17 };

struct movelist
{
    int moves[256];
    int count;

    movelist() {
        count = 0;
    }

    void add_move(int move) {
        // add move to the move list
        moves[count] = move;
        // increment move count
        count++;
    }

    void reset() {
        count = 0;
    }
};

///////////////
// functions //
///////////////

// encode moves to 21 bit representation
/* 
Move encoding

0000 0000 0000 0000 0111 1111   source square       7 bits  0x7f
0000 0000 0011 1111 1000 0000   target square       7 bits  0x7f
0000 0011 1100 0000 0000 0000   promotion piece     4 bits  0xf
0000 0100 0000 0000 0000 0000   capture flag        1 bit   0x1
0000 1000 0000 0000 0000 0000   double pawn flag    1 bit   0x1
0001 0000 0000 0000 0000 0000   en passant flag     1 bit   0x1
0010 0000 0000 0000 0000 0000   castling flag       1 bit   0x1

TODO: Test if #define macros are more performant

*/
int encode_move(
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

int decode_source(int move) {
    return move & 0x7f;
}

int decode_target(int move) {
    return (move >> 7) & 0x7f; 
}

int decode_promotion(int move) {
    return (move >> 14) & 0xf;
}

int decode_capture(int move) {
    return (move >> 18) & 0x1;
}

int decode_double_pawn(int move) {
    return (move >> 19) & 0x1;
}

int decode_enpassant(int move) {
    return (move >> 20) & 0x1;
}

int decode_castling(int move) {
    return (move >> 21) & 0x1;
}

// is given square attacked by given side
int is_square_attacked(int square, int side) {

    // pawn attacks
    if (side == white) {
        // attacker square not empty and its a correct color pawn
        if (!((square + 15) & 0x88) && (board[square + 15] == P)) {
            return 1;
        }
        if (!((square + 17) & 0x88) && (board[square + 17] == P)) {
            return 1;
        }
    } else {
        if (!((square - 15) & 0x88) && (board[square - 15] == p)) {
            return 1;
        }
        if (!((square - 17) & 0x88) && (board[square - 17] == p)) {
            return 1;
        }
    }

    // knight attacks
    for (int offset : knight_offsets) {
        // square of piece
        int target_square = square + offset;
        // piece in the square
        int target_piece = board[target_square];

        // is square on the board and is the piece correct one
        if (!(target_square & 0x88)) {
            if (side == white ? target_piece == N : target_piece == n) {
                return 1;
            }
        }
    }
    
    // king attacks
    for (int offset : king_offsets) {
        // square of piece
        int target_square = square + offset;
        // piece in the square
        int target_piece = board[target_square];

        // is square on the board and is the piece correct one
        if (!(target_square & 0x88)) {
            if (side == white ? target_piece == K : target_piece == k) {
                return 1;
            }
        }
    }

    // bishop and queen attacks
    for (int offset : bishop_offsets) {
        // square of piece
        int target_square = square + offset;
        
        // loop over attack ray
        while (!(target_square & 0x88)) {
            // target piece
            int target_piece = board[target_square];

            // is target piece white or black bishop or queen
            if (side == white ? target_piece == B || target_piece == Q : target_piece == b || target_piece == q) {
                return 1;
            }

            // break if piece is hit
            if (target_piece != e) {
                break;
            }

            // increment target square
            target_square += offset;
        }   
    }

    // rook and queen attacks
    for (int offset : rook_offsets) {
        // square of piece
        int target_square = square + offset;
        
        // loop over attack ray
        while (!(target_square & 0x88)) {
            // target piece
            int target_piece = board[target_square];

            // is target piece white or black bishop or queen
            if (side == white ? target_piece == R || target_piece == Q : target_piece == r || target_piece == q) {
                return 1;
            }

            // break if piece is hit
            if (target_piece != e) {
                break;
            }

            // increment target square
            target_square += offset;
        }   
    }

    return 0;
}

// print the chess board
void print_board() {
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

// print attacked squares
void print_attacks(int side) {
    // new line
    //cout << endl;

    cout << "    Attacking side: " << (side ? "black" : "white") << endl << endl;

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
                cout << (is_square_attacked(square, side) ? "x " : ". ");
            }
        }

        // add new line after each rank
        cout << endl;
    }

    // print file letters
    cout << endl << "    a b c d e f g h" << endl << endl;;
}

// empties the board
void reset_board() {
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

// parses fen string and sets it to board
void parse_fen(string fen) {
    reset_board();
    int i = 0;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 16; file++) {
            // square as 0..127
            int square = rank * 16 + file;

            if (!(square & 0x88)) {
                // match characters
                if ((fen[i] > 'a' && fen[i] < 'z') || (fen[i] > 'A' && fen[i] < 'Z')) {

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
}

// print board status infos
void print_board_stats() {
    cout << "    Side to move:    " << (side == white ? "white" : "black") << endl;
    cout << "    Castling rights: ";
    cout << (castle & Kc ? 'K' : '-');
    cout << (castle & Qc ? 'Q' : '-');
    cout << (castle & kc ? 'k' : '-');
    cout << (castle & qc ? 'q' : '-') << endl;
    cout << "    Enpassant:       " << (enpassant != no_sq ? square_to_coord[enpassant] : "-") << endl << endl;

}

// generate pawn moves for given side from square
void generate_pawn_moves(int side, int square, movelist &moves) {
    if (side == white) {
        // white pawn
        if (board[square] == P) {
            // non capture promotions and quiet moves
            int to_square = square - 16;
            // check if target square is on board and is empty
            if (!(to_square & 0x88) && board[to_square] == e) {
                // pawn promotions
                // is pawn on 7th rank
                if (square >= a7 && square <= h7) {
                    moves.add_move(encode_move(square, to_square, N, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, B, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, R, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, Q, 0, 0, 0, 0));
                    // cout << square_to_coord[square] << square_to_coord[to_square] << 'n' << endl;
                    // cout << square_to_coord[square] << square_to_coord[to_square] << 'b' << endl;
                    // cout << square_to_coord[square] << square_to_coord[to_square] << 'r' << endl;
                    // cout << square_to_coord[square] << square_to_coord[to_square] << 'q' << endl;
                }
                else {
                    // one square ahead pawn move
                    moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                    //cout << square_to_coord[square] << square_to_coord[to_square] << endl;

                    // two squares ahead
                    // pawn is in second rand and target square is empty
                    if ((square >= a2 && square <= h2) && board[square - 32] == e) {
                        moves.add_move(encode_move(square, square - 32, 0, 0, 1, 0, 0));
                        //cout << square_to_coord[square] << square_to_coord[square - 32] << endl;
                    }
                }
            }

            // pawn captures
            for (int offset : bishop_offsets) {
                // white pawn offsets
                if (offset < 0) {
                    // init target square
                    int to_square = square + offset;

                    // is on board and to_square 
                    if (!(to_square & 0x88)) {
                        // has enemy piece
                        if (board[to_square] >= p && board[to_square] <= k) {
                            // promotion capture
                            // is pawn on 7th rank 
                            if (square >= a7 && square <= h7) {
                                moves.add_move(encode_move(square, to_square, N, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, B, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, R, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, Q, 1, 0, 0, 0));
                                // cout << square_to_coord[square] << square_to_coord[to_square] << 'n' << endl;
                                // cout << square_to_coord[square] << square_to_coord[to_square] << 'b' << endl;
                                // cout << square_to_coord[square] << square_to_coord[to_square] << 'r' << endl;
                                // cout << square_to_coord[square] << square_to_coord[to_square] << 'q' << endl;
                            }
                            // normal capture
                            else {
                                moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                                //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                            }
                        }
                        // en passant
                        if (to_square == enpassant) {
                            moves.add_move(encode_move(square, to_square, 0, 1, 0, 1, 0));
                            cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                        }
                    }
                }
            }
        }    
    }
    else {
        // black pawn
        if (board[square] == p) {
            int to_square = square + 16;
            // check if target square is on board and is empty
            if (!(to_square & 0x88) && board[to_square] == e) {
                // pawn promotions
                // is pawn on 2nd rank
                if (square >= a2 && square <= h2) {
                    moves.add_move(encode_move(square, to_square, n, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, b, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, r, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, q, 0, 0, 0, 0));
                    // cout << square_to_coord[square] << square_to_coord[to_square] << 'n' << endl;
                    // cout << square_to_coord[square] << square_to_coord[to_square] << 'b' << endl;
                    // cout << square_to_coord[square] << square_to_coord[to_square] << 'r' << endl;
                    // cout << square_to_coord[square] << square_to_coord[to_square] << 'q' << endl;
                }
                else {
                    // one square ahead pawn move
                    moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                    //cout << square_to_coord[square] << square_to_coord[to_square] << endl;

                    // two squares ahead
                    // pawn is in 7th rand and target square is empty
                    if ((square >= a7 && square <= h7) && board[square + 32] == e) {
                        moves.add_move(encode_move(square, to_square, 0, 0, 1, 0, 0));
                        //cout << square_to_coord[square] << square_to_coord[square + 32] << endl;
                    }
                    
                }
                
            }

            // pawn captures
            for (int offset : bishop_offsets) {
                // white pawn offsets
                if (offset > 0) {
                    // init target square
                    int to_square = square + offset;

                    // is on board and to_square
                    if (!(to_square & 0x88)) {
                        // has enemy piece
                        if (board[to_square] >= P && board[to_square] <= K) {
                            // promotion capture
                            // is pawn on 2nd rank 
                            if (square >= a2 && square <= h2) {
                                moves.add_move(encode_move(square, to_square, n, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, b, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, r, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, q, 1, 0, 0, 0));
                                // cout << square_to_coord[square] << square_to_coord[to_square] << 'n' << endl;
                                // cout << square_to_coord[square] << square_to_coord[to_square] << 'b' << endl;
                                // cout << square_to_coord[square] << square_to_coord[to_square] << 'r' << endl;
                                // cout << square_to_coord[square] << square_to_coord[to_square] << 'q' << endl;
                            }
                            // normal capture
                            else {
                                moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                                //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                            }
                        }
                        // en passant
                        if (to_square == enpassant) {
                            moves.add_move(encode_move(square, to_square, 0, 1, 0, 1, 0));
                            //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                        }
                    }
                }
            }
        }
    }
}

// generate castling for given side from square
void generate_castling_moves(int side, int square, movelist &moves) {
    if (side == white) {
        // white castling
        if (board[square] == K) {
            // king side castle
            if (castle & Kc) {
                // ensure empty squares between king and rook
                if (board[f1] == e && board[g1] == e) {
                    // ensure king and next square not attacked (move gen will not allow g1 to be in check)
                    if (!is_square_attacked(square, black) && !is_square_attacked(f1, black)) {
                        moves.add_move(encode_move(e1, g1, 0, 0, 0, 0, 1));
                        //cout << square_to_coord[e1] << square_to_coord[g1] << endl;
                    }
                }
            }

            // queen side castle
            if (castle & Qc) {
                // ensure empty squares between king and rook
                if (board[d1] == e && board[c1] == e && board[b1] == e) {
                    // ensure king and next square not attacked (move gen will not allow c1 to be in check)
                    if (!is_square_attacked(square, black) && !is_square_attacked(d1, black)) {
                        moves.add_move(encode_move(e1, c1, 0, 0, 0, 0, 1));
                        //cout << square_to_coord[e1] << square_to_coord[c1] << endl;
                    }
                }
            }
        }
    }
    else {
        // black castling
        if (board[square] == k) {
            // king side castle
            if (castle & kc) {
                // ensure empty squares between king and rook
                if (board[f8] == e && board[g8] == e) {
                    // ensure king and next square not attacked (move gen will not allow g1 to be in check)
                    if (!is_square_attacked(square, white) && !is_square_attacked(f8, white)) {
                        moves.add_move(encode_move(e8, g8, 0, 0, 0, 0, 1));
                        //cout << square_to_coord[e8] << square_to_coord[g8] << endl;
                    }
                }
            }

            // queen side castle
            if (castle & qc) {
                // ensure empty squares between king and rook
                if (board[d8] == e && board[c8] == e && board[b8] == e) {
                    // ensure king and next square not attacked (move gen will not allow c1 to be in check)
                    if (!is_square_attacked(square, white) && !is_square_attacked(d8, white)) {
                        moves.add_move(encode_move(e8, c8, 0, 0, 0, 0, 1));
                        //cout << square_to_coord[e8] << square_to_coord[c8] << endl;
                    }
                }
            }
        }
    }
}

// generate knight moves
void generate_knight_moves(int side, int square, movelist &moves) {
    if (side == white ? board[square] == N : board[square] == n) {
        for (int offset : knight_offsets) {
            // init target square
            int to_square = square + offset;
            // init target piece
            int piece = board[to_square];

            // ensure target square on board
            if (!(to_square & 0x88)) {
                // ensure that target square is empty or has enemy piece
                if (side == white ? (piece == e || (piece >= p && piece <= k)) : (piece == e || (piece >= P && piece <= K))) {
                    // capture
                    if (piece != e) {
                        moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                    }
                    // quiet move
                    else {
                        moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                    }
                    
                    //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                }
            }
        }
    }
}

// generate king moves
void generate_king_moves(int side, int square, movelist &moves) {
    if (side == white ? board[square] == K : board[square] == k) {
        for (int offset : king_offsets) {
            // init target square
            int to_square = square + offset;
            // init target piece
            int piece = board[to_square];

            // ensure target square on board
            if (!(to_square & 0x88)) {
                // ensure that target square is empty or has enemy piece
                if (side == white ? (piece == e || (piece >= p && piece <= k)) : (piece == e || (piece >= P && piece <= K))) {
                    // capture
                    if (piece != e) {
                        moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                    }
                    // quiet move
                    else {
                        moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                    }
                    //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                }
            }
        }
    }
}

// generate bishop moves
void generate_bishop_moves(int side, int square, movelist &moves) {
    // correct color bishop or queen
    if (side == white ? board[square] == B || board[square] == Q : 
        board[square] == b || board[square] == q) {
        
        for (int offset : bishop_offsets) {
            // init target square
            int to_square = square + offset;

            // loop over attack ray
            while (!(to_square & 0x88)) {
                int piece = board[to_square];
                
                // hits own piece
                if (side == white ? piece >= P && piece <= K :
                    piece >= p && piece <= k) 
                {
                    break;
                }

                // hits enemy piece
                if (side == white ? piece >= p && piece <= k :
                    piece >= P && piece <= K) 
                {
                    moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));    
                    //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                    break;

                }
                // hits empty square
                if (piece == e) {
                    moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                    //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                }

                // increment target square
                to_square += offset;
            }
        }
    }
}

// generate rook moves
void generate_rook_moves(int side, int square, movelist &moves) {
    // correct color rook or queen
    if (side == white ? board[square] == R || board[square] == Q : 
        board[square] == r || board[square] == q) {
        
        for (int offset : rook_offsets) {
            // init target square
            int to_square = square + offset;

            // loop over attack ray
            while (!(to_square & 0x88)) {
                int piece = board[to_square];
                
                // hits own piece
                if (side == white ? piece >= P && piece <= K :
                    piece >= p && piece <= k) 
                {
                    break;
                }

                // hits enemy piece
                if (side == white ? piece >= p && piece <= k :
                    piece >= P && piece <= K) 
                {
                    moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                    //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                    break;

                }
                // hits empty square
                if (piece == e) {
                    moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                    //cout << square_to_coord[square] << square_to_coord[to_square] << endl;
                }

                // increment target square
                to_square += offset;
            }
        }
    }
}

// move generator
void generate_moves(movelist &moves) {

    // empty out movelist
    moves.reset();

    // loop over all squares of the board
    for (int square = 0; square < 128; square++) {
        // is square on board
        if (!(square & 0x88)) {
            generate_pawn_moves(side, square, moves);
            generate_castling_moves(side, square, moves);
            generate_knight_moves(side, square, moves);
            generate_king_moves(side, square, moves);
            generate_bishop_moves(side, square, moves);
            generate_rook_moves(side, square, moves);
        }
    }
}

void print_movelist(movelist &moves) {
    for (int i = 0; i < moves.count; i++) {
        cout << square_to_coord[decode_source(moves.moves[i])];
        cout << square_to_coord[decode_target(moves.moves[i])] << " ";
        cout << (decode_promotion(moves.moves[i]) ? ascii_pieces[decode_promotion(moves.moves[i])] : '-') << " ";
        cout << (decode_capture(moves.moves[i]) ? 'x' : '-') << " ";
        cout << (decode_double_pawn(moves.moves[i]) ? 'd' : '-') << " ";
        cout << (decode_enpassant(moves.moves[i]) ? 'e' : '-') << " ";
        cout << (decode_castling(moves.moves[i]) ? 'c' : '-') << endl;
    }

    cout << "Total move count: " << moves.count << endl;
}


int main() {
    //cout << "sq: " + to_string(e4) + " coord: " + square_to_coord[e4];

    parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    //parse_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    print_board();
    print_board_stats();
    //print_attacks(white);

    movelist moves;
    generate_moves(moves);

    //print_movelist(moves);
}