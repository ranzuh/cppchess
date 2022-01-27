#include <cstring>
#include <vector>
#include <chrono>
#include "position.h"
#include "movegen.h"
#include "uci.h"

////////////
//  UCI   //
////////////

// parse user/gui move string input (eg e7e8q)
int parse_move(Position &pos, string move_string) {
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
        pos.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    // parse fen string
    else if (command.find("fen") != string::npos) {
        pos.parse_fen(command.substr(command.find("fen") + 4));
    }

    // parse moves
    if (command.find("moves") != string::npos) {
        pos.rep_index = 0;
        memset(pos.rep_stack, 0, sizeof(pos.rep_stack));

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

void search_position(Position &pos, int depth);

// parse UCI "go" command
void parse_go(Position &pos, string command) {
    int depth = -1;

    if (command.find("depth") != string::npos) {
        int index = command.find("depth");
        depth = stoi(command.substr(index + 6));
    }
    else {
        depth = 10;
    }

    // start timing
    stopped = 0;
    seconds_per_move = 5;
    go_start = chrono::steady_clock::now();

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