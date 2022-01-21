// based on tutorial by Chess Programming @ youtube (https://www.youtube.com/playlist?list=PLmN0neTso3JzhJP35hwPHJi4FZgw5Ior0)

#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include "position.h"
#include "movegen.h"
#include <cassert>
#include <vector>
#include <climits>

using namespace std;

string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
string tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

// perft test
uint64_t perft(Position &pos, int depth, bool divide) {
    Movelist moves;
    int n_moves;
    uint64_t nodes = 0;

    n_moves = generate_legal_moves(pos, moves);
    
    if (depth == 1) return n_moves;

    //generate_moves(moves);

    // // define board state copies
    // int board_copy[128], king_squares_copy[2], side_copy, enpassant_copy, castle_copy;
    // // copy board state
    // copy(pos.board, pos.board + 128, board_copy);
    // copy(pos.king_squares, pos.king_squares + 2, king_squares_copy);
    // side_copy = pos.side;
    // enpassant_copy = pos.enpassant;
    // castle_copy = pos.castle;

    // copy board state
    Position copy = pos;
    
    for (int i = 0; i < moves.count; i++) {
        //print_board();
        if (pos.make_move(moves.moves[i])) {
            // pos.print_board();
            // getchar();
            uint64_t result = perft(pos, depth - 1, false);
            nodes += result;
            if (divide) {
                cout << Position::square_to_coord[decode_source(moves.moves[i])];
                cout << Position::square_to_coord[decode_target(moves.moves[i])];
                cout << " " << result << endl;
            }
            

            // // restore board state
            // copy(board_copy, board_copy + 128, pos.board);
            // copy(king_squares_copy, king_squares_copy + 2, pos.king_squares);
            // pos.side = side_copy;
            // pos.enpassant = enpassant_copy;
            // pos.castle = castle_copy;

            // restore board state
            pos = copy;

            // pos.print_board();
            // getchar();
        }
    }

    return nodes;
}

void run_perft(Position &pos, int depth) {
    auto start = chrono::high_resolution_clock::now();
    uint64_t result = perft(pos, depth, true);
    auto stop = chrono::high_resolution_clock::now();
    
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    cout << endl;
    cout << "Nodes visited:    " << result << endl;
    cout << "Total time:       " << duration.count() << " ms" << endl;
    cout << fixed << "Nodes per second: " << int(1000.0 * result / duration.count()) << endl;
}

void run_perft_tests() {
    Position pos;
    uint64_t result;

    // test 1
    cout << "Test 1: Start position" << endl;
    pos.parse_fen(start_position);
    result = perft(pos, 5, true);
    assert(result ==  4865609);


    // test 2
    cout << endl << "Test 2: Kiwipete" << endl;
    pos.parse_fen(tricky_position);
    result = perft(pos, 4, true);
    assert(result ==  4085603);

    // test 3
    cout << endl << "Test 3: Position 3" << endl;
    pos.parse_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    result = perft(pos, 5, true);
    assert(result == 674624);

    // test 4
    cout << endl << "Test 4: Position 4" << endl;
    pos.parse_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    result = perft(pos, 4, true);
    assert(result == 422333);

    // test 5
    cout << endl << "Test 5: Position 5" << endl;
    pos.parse_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    result = perft(pos, 4, true);
    assert(result ==  2103487);

    cout << endl << "Perft tests passed" << endl;
}

/////////////////////////
// search & evaluation //
/////////////////////////

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
            if (!(square & 0x88)) {
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

uint64_t nodes = 0;

uint64_t quiesc_nodes = 0;

int ply = 0;

int quiescence_search(Position &pos, int alpha, int beta) {
    // get lower bound score
    int stand_pat = evaluate_position(pos);

    if (stand_pat >= beta) {
        return beta;
    }

    if (stand_pat > alpha) {
        alpha = stand_pat;
    }

    // copy board state
    Position copy = pos;

    // loop over all captures
    Movelist moves;
    generate_legal_moves(pos, moves);

    for (int i = 0; i < moves.count; i++) {
        if (decode_capture(moves.moves[i])) {
            if (pos.make_move(moves.moves[i])) {
                nodes++;
                quiesc_nodes++;
                ply++;
                int value = -quiescence_search(pos, -beta, -alpha);
                ply--;
                if (value >= beta) {
                    return beta;
                }
                // restore board state
                pos = copy;

                if (value > alpha) {
                    alpha = value;
                }
            }
        }
    }

    return alpha;
}

// color +1 for white, -1 for black
int negamax(Position &pos, int depth, int alpha, int beta) {
    if (depth == 0) {
        //nodes += 1;
        return quiescence_search(pos, alpha, beta);
        //return evaluate_position(pos);
    }
    int value = -500000;

    Movelist moves;
    generate_legal_moves(pos, moves);

    int king_attacked = is_square_attacked(pos, pos.king_squares[pos.side], !pos.side);

    // if no legal moves
    if (moves.count == 0) {
        // king is in check - checkmate
        if (king_attacked) {
            //cout << "found mate at ply " << ply << endl;
            //nodes += 1;
            return -50000 + ply;
        }
        // king is not in check - stalemate
        else {
            //cout << "found stalemate at ply " << ply << endl;
            //nodes += 1;
            return 0;
        }
    }

    // copy board state
    Position copy = pos;

    for (int i = 0; i < moves.count; i++) {
        if (pos.make_move(moves.moves[i])) {
            nodes++;
            ply++;  
            //value = max(value, -negamax(pos, depth - 1, -beta, -alpha));
            value = -negamax(pos, depth - 1, -beta, -alpha);
            ply--;
            //alpha = max(alpha, value);
            if (value >= beta) {
                return beta;
            }
            // restore board state
            pos = copy;
            // if (alpha >= beta) {
            //     break;
            // }
            if (value > alpha) {
                alpha = value;
            }
        }
    }

    return alpha;
}

// search position for the best move
void search_position(Position &pos, int depth) {
    // best move placeholder
    nodes = 0;
    quiesc_nodes = 0;
    int value = 0;
    int best_move = 0;
    int best_value = -500000;

    // color for negamax calculation
    //int color = pos.side == white ? 1 : -1;

    Movelist moves;
    generate_legal_moves(pos, moves);

    // copy board state
    Position copy = pos;

    // start timing
    auto start = chrono::high_resolution_clock::now();

    // iterative deepening
    //for (int iter_depth = 1; iter_depth <= depth; iter_depth++) {

        // go through legal moves
        for (int i = 0; i < moves.count; i++) {
            if (pos.make_move(moves.moves[i])) {
                nodes++;
                //cout << i << endl;
                ply++;
                value = -negamax(pos, depth - 1, -500000, 500000);
                ply--;
                // restore board state
                pos = copy;

                if (value > best_value) {
                    best_move = moves.moves[i];
                    best_value = value;
                }
            }
        }
        // stop clock
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

        // info depth 2 score cp 214 time 1242 nodes 2124 nps 34928
        cout << "info depth " << depth << " score cp " << best_value << " nodes " << nodes << " nps " << max(0, int(1000.0 * nodes / duration.count())) << " pv " << endl;
    //}
    cout << "bestmove " << Position::square_to_coord[decode_source(best_move)] << Position::square_to_coord[decode_target(best_move)];
    if (decode_promotion(best_move)) {
        int promoted_piece = decode_promotion(best_move);
        // promoted to queen
        if ((promoted_piece == Q || promoted_piece == q))
            cout << 'q';
        
        // promoted to rook
        else if ((promoted_piece == R || promoted_piece == r))
            cout << 'r';
        
        // promoted to bishop
        else if ((promoted_piece == B || promoted_piece == b))
            cout << 'b';
        
        // promoted to knight
        else if ((promoted_piece == N || promoted_piece == n))
            cout << 'n';
    }
    cout << endl;
}

////////////
//  UCI   //
////////////

// parse user/gui move string input (eg e7e8q)
int parse_move (Position &pos, string move_string) {
    // parse squares from move string
    int from_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 16;
    int to_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 16;

    // generate pseudolegal moves
    Movelist moves;
    generate_legal_moves(pos, moves);

    for (int i = 0; i < moves.count; i++)
    {
        int move = moves.moves[i];

        if (from_square == decode_source(move) && to_square == decode_target(move)) {
            int promoted_piece = decode_promotion(move);

            if (promoted_piece) {
                // promoted to queen
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q')
                    return move;
                
                // promoted to rook
                else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r')
                    return move;
                
                // promoted to bishop
                else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b')
                    return move;
                
                // promoted to knight
                else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n')
                    return move;
                
                // continue the loop on possible wrong promotions (e.g. "e7e8f")
                continue;
            }

            // return legal move
            return move;
        }
    }
    
    // move not legal
    return 0;
}

/*
    Example UCI commands to init position on chess board
    
    // init start position
    position startpos
    
    // init start position and make the moves on chess board
    position startpos moves e2e4 e7e5
    
    // init position from FEN string
    position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 
    
    // init position from fen string and make moves on chess board
    position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 moves e2a6 e8g8
*/

// parse UCI "position" command
void parse_position(Position &pos, string command) {
    // parse startpos command
    if (command.find("startpos") != string::npos) {
        pos.parse_fen(start_position);
    }
    // parse fen string
    else if (command.find("fen") != string::npos) {
        pos.parse_fen(command.substr(command.find("fen") + 4));
    }

    // parse moves
    if (command.find("moves") != string::npos) {
        string moves_substring = command.substr(command.find("moves") + 6);

        // make vector of parsed moves
        vector<string> moves;
        string move = "";
        for (char c : moves_substring) {
            if (c == ' ') {
                moves.push_back(move);
                move = "";
            }
            else {
                move += c;
            }
        }
        // push last move to vector
        moves.push_back(moves_substring.substr(moves_substring.find_last_of(' ') + 1));

        // make moves
        for (string s : moves) {
            int parsed_move = parse_move(pos, s);
            //cout << s << endl;
            if (parsed_move) {
                pos.make_move(parsed_move);
            }
            else {
                cout << "illegal move" << endl;
            }
        }
        
    }
}

/*
    Example UCI commands to make engine search for the best move
    
    // fixed depth search
    go depth 64
*/

// parse UCI "go" command
void parse_go(Position &pos, string command) {
    int depth = -1;

    if (command.find("depth") != string::npos) {
        int index = command.find("depth");
        depth = stoi(command.substr(index + 6));
    }
    else {
        depth = 5;
    }

    // todo time control

    //cout << "Depth: " << depth << endl;

    // search position
    search_position(pos, depth);
}

/*
    GUI -> isready
    Engine -> readyok
    GUI -> ucinewgame
*/
// main uci loop
void uci_loop(Position &pos) {
    // setbuf(stdin, NULL);
    // setbuf(stdout, NULL);

    cout << "id name cppchess" << endl;
    cout << "id author Eetu Rantala" << endl;
    cout << "uciok" << endl;

    string input;

    while(1) {
        // reset user /GUI input
        
        // make sure output reaches the GUI
        
        // get user / GUI input
        getline(cin, input);
        
        // make sure input is available
        
        
        // parse UCI "isready" command
        if (input.find("isready") != string::npos) {
            cout << "readyok" << endl;
            continue;
        }
        
        // parse UCI "position" command
        else if (input.find("position") != string::npos) {
            parse_position(pos, input);
            pos.print_board();
            pos.print_board_stats();
        }
        
        // parse UCI "ucinewgame" command
        else if (input.find("ucinewgame") != string::npos) {
            parse_position(pos, "position startpos");
            pos.print_board();
            pos.print_board_stats();
        }
        
        // parse UCI "go" command
        else if (input.find("go") != string::npos) {
            parse_go(pos, input);
            //continue;
        }
        
        // parse UCI "quit" command
        else if (input.find("quit") != string::npos) {
            break;
        }
        
        // parse UCI "uci" command
        else if (input.find("uci") != string::npos) {
            cout << "id name cppchess" << endl;
            cout << "id author Eetu Rantala" << endl;
            cout << "uciok" << endl;
        }
    }
}

int main() {
    Position game_position;

    game_position.parse_fen("rnb1kbnr/1pppqppp/p7/4N3/4P3/7P/PPPP1PP1/RNBQKB1R b KQkq - 0 4"); 
    

    
    //run_perft(game_position, 6);

    int debug = 1;

    if (debug) {
        parse_position(game_position, "position fen 8/4k3/8/8/8/p7/4K3/8 w - - 0 1");
        game_position.print_board();
        game_position.print_board_stats();

        // Movelist moves;
        // generate_legal_moves(game_position, moves);

        // Position copy = game_position;

        // for (int i = 0; i < moves.count; i++)
        // {
        //     cout << "move " << Position::square_to_coord[decode_source(moves.moves[i])] << Position::square_to_coord[decode_target(moves.moves[i])];
        //     game_position.make_move(moves.moves[i]);

        //     cout << " score " << -negamax(game_position, 5, -500000, 500000) << endl;

        //     game_position = copy;
        // }
        //search_position(game_position, 6);
        //cout << "Quiescence search nodes: " << quiesc_nodes << endl;
        //run_perft(game_position, 5);

        cout << evaluate_position(game_position) << endl;
    }
    else {
        uci_loop(game_position);
    }

    
}