#include <cstring>
#include <vector>
#include <chrono>
#include "position.h"
#include "movegen.h"
#include "uci.h"
#include "hashtable.h"

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
                pos.make_move(parsed_move, 0);
                pos.rep_index += 1;
                pos.rep_stack[pos.rep_index] = pos.hash_key;
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

int time_set = 0;
chrono::steady_clock::time_point stop_time;
int time_left = -1;
chrono::steady_clock::time_point go_start;
int stopped = 0;
int seconds_per_move = 5;

void reset_time_control() {
    stopped = 0;
    time_set = 0;
    stop_time = chrono::steady_clock::now();
    time_left = -1;
}

void check_time() {
    if (time_set) {
        if (chrono::steady_clock::now() > stop_time) {
            //cout << "5 sec passed" << endl;
            stopped = 1;
        }
    }
}

// parse UCI "go" command
void parse_go(Position &pos, string command) {
    int depth = -1;

    reset_time_control();

    uint64_t increment = 0;
    uint64_t move_time = -1;

    if (command.find("wtime") != string::npos && pos.side == white) {
        string moves_substring = command.substr(command.find("wtime") + 6);
        string wtime_string = moves_substring.substr(0, moves_substring.find(" "));
        time_left = stoi(wtime_string);
        //cout << "wtime: " << time_left << endl;
    }
    if (command.find("btime") != string::npos && pos.side == black) {
        string moves_substring = command.substr(command.find("btime") + 6);
        string btime_string = moves_substring.substr(0, moves_substring.find(" "));
        time_left = stoi(btime_string);
        //cout << "btime: " << time_left << endl;
    }
    if (command.find("winc") != string::npos && pos.side == white) {
        string moves_substring = command.substr(command.find("winc") + 5);
        string winc_string = moves_substring.substr(0, moves_substring.find(" "));
        increment = stoi(winc_string);
        //cout << "winc: " << increment << endl;
    }
    if (command.find("binc") != string::npos && pos.side == black) {
        string moves_substring = command.substr(command.find("binc") + 5);
        string binc_string = moves_substring.substr(0, moves_substring.find(" "));
        increment = stoi(binc_string);
        //cout << "binc: " << increment << endl;
    }
    if (command.find("movetime") != string::npos) {
        string moves_substring = command.substr(command.find("movetime") + 9);
        string movetime_string = moves_substring.substr(0, moves_substring.find(" "));
        move_time = stoi(movetime_string);
        time_left = move_time;
        cout << "move_time: " << move_time << endl;
    }

    if (time_left != -1) {
        time_set = 1;
    }

    if (move_time == -1) {
        move_time = time_left / 40;
        move_time += increment / 2;

        if (move_time >= time_left) 
            move_time -= 500;
        
        if (move_time <= 0)
            move_time = 100;
    }
    
    auto start_time = chrono::steady_clock::now();
    auto move_time_chrono = chrono::milliseconds(move_time);
    stop_time = start_time + move_time_chrono;

    if (command.find("depth") != string::npos) {
        int index = command.find("depth");
        depth = stoi(command.substr(index + 6));
    }
    else {
        depth = 64;
    }

    // search position
    search_position(pos, depth);
}

/*
    GUI -> isready
    Engine -> readyok
    GUI -> ucinewgame
*/
// main uci loop
void uci_loop() {
    // setbuf(stdin, NULL);
    // setbuf(stdout, NULL);
    Position pos;
    clear_hash_table();

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
            Position new_pos;
            pos =  new_pos;
            parse_position(pos, "position startpos");
            clear_hash_table();
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