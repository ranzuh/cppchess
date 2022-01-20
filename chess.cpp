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
uint64_t perft(Position pos, int depth, bool divide) {
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

void run_perft(Position pos, int depth) {
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

int evaluate_position(Position &pos) {
    // int score = 0;
    // for (int square = 0; square < 128; square++) {
    //     if (!(square & 0x88)) {
    //         score += piece_values[pos.board[square]];
    //         //cout << pos.board[square] << " " << piece_values[pos.board[square]] << endl;
    //     }
    // }
    // assert(pos.side == white ? pos.material_score == score : -pos.material_score == -score);
    return pos.side == white ? pos.material_score : -pos.material_score;
}

int nodes = 0;

// color +1 for white, -1 for black
int negamax(Position &pos, int depth) {
    if (depth == 0) {
        nodes += 1;
        return evaluate_position(pos);
    }
    int value = INT_MIN;

    Movelist moves;
    generate_legal_moves(pos, moves);

    // copy board state
    Position copy = pos;

    for (int i = 0; i < moves.count; i++) {
        if (pos.make_move(moves.moves[i])) {
            value = max(value, -negamax(pos, depth - 1));

            // restore board state
            pos = copy;
        }
    }

    return value;
}

// search position for the best move
void search_position(Position &pos, int depth) {
    // best move placeholder
    int value = 0;
    int best_move = 0;
    int best_value = INT_MIN;

    // color for negamax calculation
    int color = pos.side == white ? 1 : -1;

    Movelist moves;
    generate_legal_moves(pos, moves);

    // copy board state
    Position copy = pos;

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < moves.count; i++) {
        if (pos.make_move(moves.moves[i])) {
            //cout << i << endl;
            value = -negamax(pos, depth - 1);
            
            // restore board state
            pos = copy;

            if (value > best_value) {
                best_move = moves.moves[i];
                best_value = value;
            }
        }
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    // info depth 2 score cp 214 time 1242 nodes 2124 nps 34928
    cout << "info depth " << depth << " nodes " << nodes << " nps " << int(1000.0 * nodes / duration.count()) << endl;

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
        depth = 4;
    }

    // todo time control

    cout << "Depth: " << depth << endl;

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
        parse_position(game_position, "position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
        game_position.print_board();
        game_position.print_board_stats();

        // Movelist moves;
        // generate_legal_moves(game_position, moves);

        // Position copy = game_position;

        // for (int i = 0; i < moves.count; i++)
        // {
        //     cout << "move " << Position::square_to_coord[decode_source(moves.moves[i])] << Position::square_to_coord[decode_target(moves.moves[i])];
        //     game_position.make_move(moves.moves[i]);

        //     cout << " score " << -negamax(game_position, 0) << endl;

        //     game_position = copy;
        // }
        search_position(game_position, 4);
        cout << nodes << endl;
    }
    else {
        uci_loop(game_position);
    }

    
}