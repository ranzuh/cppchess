#include <iostream>
#include <string>
#include <map>
#include "position.h"
#include "movegen.h"

using namespace std;

// piece move offsets

int knight_offsets[] = { -33, -31, -18, -14,  14, 18, 31, 33 };
int bishop_offsets[] = { -17, -15, 15, 17 };
int rook_offsets[] = { -16, -1, 1, 16 };
int king_offsets[] = { -17, -16, -15, -1, 1, 15, 16, 17 };

void Movelist::add_move(int move) {
    // add move to the move list
    moves[count] = move;
    // increment move count
    count++;
}

void Movelist::reset() {
    count = 0;
}

// is given square attacked by given side
int is_square_attacked(Position &pos, int square, int side) {

    // pawn attacks
    if (side == white) {
        // attacker square not empty and its a correct color pawn
        if (!((square + 15) & 0x88) && (pos.board[square + 15] == P)) {
            return 1;
        }
        if (!((square + 17) & 0x88) && (pos.board[square + 17] == P)) {
            return 1;
        }
    } else {
        if (!((square - 15) & 0x88) && (pos.board[square - 15] == p)) {
            return 1;
        }
        if (!((square - 17) & 0x88) && (pos.board[square - 17] == p)) {
            return 1;
        }
    }

    // knight attacks
    for (int offset : knight_offsets) {
        // square of piece
        int target_square = square + offset;
        // piece in the square
        int target_piece = pos.board[target_square];

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
        int target_piece = pos.board[target_square];

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
            int target_piece = pos.board[target_square];

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
            int target_piece = pos.board[target_square];

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

// generate pawn moves for given side from square
void generate_pawn_moves(Position &pos, int square, Movelist &moves) {
    if (pos.side == white) {
        // white pawn
        if (pos.board[square] == P) {
            // non capture promotions and quiet moves
            int to_square = square - 16;
            // check if target square is on board and is empty
            if (!(to_square & 0x88) && pos.board[to_square] == e) {
                // pawn promotions
                // is pawn on 7th rank
                if (square >= a7 && square <= h7) {
                    moves.add_move(encode_move(square, to_square, N, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, B, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, R, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, Q, 0, 0, 0, 0));
                }
                else {
                    // one square ahead pawn move
                    moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));

                    // two squares ahead
                    // pawn is in second rand and target square is empty
                    if ((square >= a2 && square <= h2) && pos.board[square - 32] == e) {
                        moves.add_move(encode_move(square, square - 32, 0, 0, 1, 0, 0));
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
                        if (pos.board[to_square] >= p && pos.board[to_square] <= k) {
                            // promotion capture
                            // is pawn on 7th rank 
                            if (square >= a7 && square <= h7) {
                                moves.add_move(encode_move(square, to_square, N, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, B, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, R, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, Q, 1, 0, 0, 0));
                            }
                            // normal capture
                            else {
                                moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                            }
                        }
                        // en passant
                        if (to_square == pos.enpassant) {
                            moves.add_move(encode_move(square, to_square, 0, 1, 0, 1, 0));
                        }
                    }
                }
            }
        }    
    }
    else {
        // black pawn
        if (pos.board[square] == p) {
            int to_square = square + 16;
            // check if target square is on board and is empty
            if (!(to_square & 0x88) && pos.board[to_square] == e) {
                // pawn promotions
                // is pawn on 2nd rank
                if (square >= a2 && square <= h2) {
                    moves.add_move(encode_move(square, to_square, n, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, b, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, r, 0, 0, 0, 0));
                    moves.add_move(encode_move(square, to_square, q, 0, 0, 0, 0));
                }
                else {
                    // one square ahead pawn move
                    moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));

                    // two squares ahead
                    // pawn is in 7th rand and target square is empty
                    if ((square >= a7 && square <= h7) && pos.board[square + 32] == e) {
                        moves.add_move(encode_move(square, square + 32, 0, 0, 1, 0, 0));
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
                        if (pos.board[to_square] >= P && pos.board[to_square] <= K) {
                            // promotion capture
                            // is pawn on 2nd rank 
                            if (square >= a2 && square <= h2) {
                                moves.add_move(encode_move(square, to_square, n, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, b, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, r, 1, 0, 0, 0));
                                moves.add_move(encode_move(square, to_square, q, 1, 0, 0, 0));
                            }
                            // normal capture
                            else {
                                moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                            }
                        }
                        // en passant
                        if (to_square == pos.enpassant) {
                            moves.add_move(encode_move(square, to_square, 0, 1, 0, 1, 0));
                        }
                    }
                }
            }
        }
    }
}

// generate castling for given side from square
void generate_castling_moves(Position &pos, int square, Movelist &moves) {
    if (pos.side == white) {
        // white castling
        if (pos.board[square] == K) {
            // king side castle
            if (pos.castle & Kc) {
                // ensure empty squares between king and rook
                if (pos.board[f1] == e && pos.board[g1] == e) {
                    // ensure king and next square not attacked (move gen will not allow g1 to be in check)
                    if (!is_square_attacked(pos, square, black) && !is_square_attacked(pos, f1, black)) {
                        moves.add_move(encode_move(e1, g1, 0, 0, 0, 0, 1));
                    }
                }
            }

            // queen side castle
            if (pos.castle & Qc) {
                // ensure empty squares between king and rook
                if (pos.board[d1] == e && pos.board[c1] == e && pos.board[b1] == e) {
                    // ensure king and next square not attacked (move gen will not allow c1 to be in check)
                    if (!is_square_attacked(pos, square, black) && !is_square_attacked(pos, d1, black)) {
                        moves.add_move(encode_move(e1, c1, 0, 0, 0, 0, 1));
                    }
                }
            }
        }
    }
    else {
        // black castling
        if (pos.board[square] == k) {
            // king side castle
            if (pos.castle & kc) {
                // ensure empty squares between king and rook
                if (pos.board[f8] == e && pos.board[g8] == e) {
                    // ensure king and next square not attacked (move gen will not allow g1 to be in check)
                    if (!is_square_attacked(pos, square, white) && !is_square_attacked(pos, f8, white)) {
                        moves.add_move(encode_move(e8, g8, 0, 0, 0, 0, 1));
                    }
                }
            }

            // queen side castle
            if (pos.castle & qc) {
                // ensure empty squares between king and rook
                if (pos.board[d8] == e && pos.board[c8] == e && pos.board[b8] == e) {
                    // ensure king and next square not attacked (move gen will not allow c1 to be in check)
                    if (!is_square_attacked(pos, square, white) && !is_square_attacked(pos, d8, white)) {
                        moves.add_move(encode_move(e8, c8, 0, 0, 0, 0, 1));
                    }
                }
            }
        }
    }
}

// generate knight moves
void generate_knight_moves(Position &pos, int square, Movelist &moves) {
    if (pos.side == white ? pos.board[square] == N : pos.board[square] == n) {
        for (int offset : knight_offsets) {
            // init target square
            int to_square = square + offset;
            // init target piece
            int piece = pos.board[to_square];

            // ensure target square on board
            if (!(to_square & 0x88)) {
                // ensure that target square is empty or has enemy piece
                if (pos.side == white ? (piece == e || (piece >= p && piece <= k)) : (piece == e || (piece >= P && piece <= K))) {
                    // capture
                    if (piece != e) {
                        moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                    }
                    // quiet move
                    else {
                        moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                    }
                }
            }
        }
    }
}

// generate king moves
void generate_king_moves(Position &pos, int square, Movelist &moves) {
    if (pos.side == white ? pos.board[square] == K : pos.board[square] == k) {
        for (int offset : king_offsets) {
            // init target square
            int to_square = square + offset;
            // init target piece
            int piece = pos.board[to_square];

            // ensure target square on board
            if (!(to_square & 0x88)) {
                // ensure that target square is empty or has enemy piece
                if (pos.side == white ? (piece == e || (piece >= p && piece <= k)) : (piece == e || (piece >= P && piece <= K))) {
                    // capture
                    if (piece != e) {
                        moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                    }
                    // quiet move
                    else {
                        moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                    }
                }
            }
        }
    }
}

// generate bishop moves
void generate_bishop_moves(Position &pos, int square, Movelist &moves) {
    // correct color bishop or queen
    if (pos.side == white ? pos.board[square] == B || pos.board[square] == Q : 
        pos.board[square] == b || pos.board[square] == q) {
        
        for (int offset : bishop_offsets) {
            // init target square
            int to_square = square + offset;

            // loop over attack ray
            while (!(to_square & 0x88)) {
                int piece = pos.board[to_square];
                
                // hits own piece
                if (pos.side == white ? piece >= P && piece <= K :
                    piece >= p && piece <= k) 
                {
                    break;
                }

                // hits enemy piece
                if (pos.side == white ? piece >= p && piece <= k :
                    piece >= P && piece <= K) 
                {
                    moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));    
                    break;

                }
                // hits empty square
                if (piece == e) {
                    moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                }

                // increment target square
                to_square += offset;
            }
        }
    }
}

// generate rook moves
void generate_rook_moves(Position &pos, int square, Movelist &moves) {
    // correct color rook or queen
    if (pos.side == white ? pos.board[square] == R || pos.board[square] == Q : 
        pos.board[square] == r || pos.board[square] == q) {
        
        for (int offset : rook_offsets) {
            // init target square
            int to_square = square + offset;

            // loop over attack ray
            while (!(to_square & 0x88)) {
                int piece = pos.board[to_square];
                
                // hits own piece
                if (pos.side == white ? piece >= P && piece <= K :
                    piece >= p && piece <= k) 
                {
                    break;
                }

                // hits enemy piece
                if (pos.side == white ? piece >= p && piece <= k :
                    piece >= P && piece <= K) 
                {
                    moves.add_move(encode_move(square, to_square, 0, 1, 0, 0, 0));
                    break;

                }
                // hits empty square
                if (piece == e) {
                    moves.add_move(encode_move(square, to_square, 0, 0, 0, 0, 0));
                }

                // increment target square
                to_square += offset;
            }
        }
    }
}

// pseudo-legal move generator, may leave king under check
int generate_pseudo_moves(Position &pos, Movelist &moves) {

    // empty out movelist
    moves.reset();

    // loop over all squares of the board
    for (int square = 0; square < 128; square++) {
        // is square on board
        if (!(square & 0x88)) {
            generate_pawn_moves(pos, square, moves);
            generate_castling_moves(pos, square, moves);
            generate_knight_moves(pos, square, moves);
            generate_king_moves(pos, square, moves);
            generate_bishop_moves(pos, square, moves);
            generate_rook_moves(pos, square, moves);
        }
    }

    return moves.count;
}

// in current sides king attacked after given move
int is_king_attacked(Position &pos, int move) {
    int from_square = decode_source(move);
    int to_square = decode_target(move);
    int is_enpassant = decode_enpassant(move);

    //Position copy = pos;

    // make move and copy board states
    int square_copy = pos.board[to_square];
    int king_copy = pos.king_squares[pos.side];
    pos.board[to_square] = pos.board[from_square];
    pos.board[from_square] = e;
    int enpass_piece = 0;

    if (is_enpassant) {
        pos.side == white ? enpass_piece = pos.board[to_square + 16] : enpass_piece = pos.board[to_square - 16];
        pos.side == white ? pos.board[to_square + 16] = e : pos.board[to_square - 16] = e;
    }

    // update king square
    if (pos.board[to_square] == K || pos.board[to_square] == k) {
        pos.king_squares[pos.side] = to_square;
    }

    int attacked = is_square_attacked(pos, pos.king_squares[pos.side], !pos.side);

    //pos = copy;
    
    // restore board state
    pos.king_squares[pos.side] = king_copy;
    pos.board[from_square] = pos.board[to_square];
    pos.board[to_square] = square_copy;

    if (is_enpassant) {
        pos.side == white ? pos.board[to_square + 16] = enpass_piece : pos.board[to_square - 16] = enpass_piece;
    }

    return attacked;
}

// legal move generator
int generate_legal_moves(Position &pos, Movelist &moves) {
    // empty out movelist
    moves.reset();
    Movelist pseudo_moves;
    generate_pseudo_moves(pos, pseudo_moves);

    for (int i = 0; i < pseudo_moves.count; i++) {
        if (!is_king_attacked(pos, pseudo_moves.moves[i])) {
            moves.add_move(pseudo_moves.moves[i]);
        }
    }

    return moves.count;
}

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

void print_movelist(Movelist &moves) {
    for (int i = 0; i < moves.count; i++) {
        cout << Position::square_to_coord[decode_source(moves.moves[i])];
        cout << Position::square_to_coord[decode_target(moves.moves[i])] << " ";
        cout << (decode_promotion(moves.moves[i]) ? Position::ascii_pieces[decode_promotion(moves.moves[i])] : '-') << " ";
        cout << (decode_capture(moves.moves[i]) ? 'x' : '-') << " ";
        cout << (decode_double_pawn(moves.moves[i]) ? 'd' : '-') << " ";
        cout << (decode_enpassant(moves.moves[i]) ? 'e' : '-') << " ";
        cout << (decode_castling(moves.moves[i]) ? 'c' : '-') << endl;
    }

    cout << "Total move count: " << moves.count << endl;
}

string get_move_string(int move) {
    string move_string = "";
    move_string += Position::square_to_coord[decode_source(move)] + Position::square_to_coord[decode_target(move)];
    if (decode_promotion(move)) {
        int promoted_piece = decode_promotion(move);
        // promoted to queen
        if ((promoted_piece == Q || promoted_piece == q))
            move_string += 'q';
        
        // promoted to rook
        else if ((promoted_piece == R || promoted_piece == r))
            move_string += 'r';
        
        // promoted to bishop
        else if ((promoted_piece == B || promoted_piece == b))
            move_string += 'b';
        
        // promoted to knight
        else if ((promoted_piece == N || promoted_piece == n))
            move_string += 'n';
    }
    return move_string;
}