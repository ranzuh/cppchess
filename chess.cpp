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
#include <algorithm>
#include <cstring>
#include "hashtable.h"
#include "uci.h"
#include "search.h"
#include "perft.h"

using namespace std;

string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
string tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

int main() {
    init_random_keys();
    Position game_position;
    clear_hash_table();

    int debug = 0;

    if (debug) {
        // game_position.parse_fen("6rk/8/8/8/8/8/8/RK6 w - - 0 1");
        // game_position.print_board();
        // game_position.print_board_stats();
        // parse_position(game_position, "position fen 8/6k1/5p2/6p1/2P4p/P2Q3P/2P2PP1/4q1K1 w - - 3 41");
        // parse_position(game_position, "position fen 8/6k1/5p2/6p1/2P4p/P2Q3P/2P2PP1/4q1K1 w - - 3 41 moves d3f1 e1c3");
        game_position.parse_fen(tricky_position);
        game_position.print_board();
        game_position.print_board_stats();

        seconds_per_move = 10;
        stopped = 0;
        go_start = chrono::steady_clock::now();

        search_position(game_position, 10);
        cout << "Quiescence search nodes: " << quiesc_nodes << endl;
        cout << "Table hits:              " << table_hits << endl;
        cout << "Call count:              " << call_count << endl;

        // string s = "";
        // for (int i = 0; i < pv_length[0]; i++) {
        //     assert(parse_move(game_position, get_move_string(pv_table[0][i])) != 0);
        //     game_position.make_move(pv_table[0][i]);
        // }

        // call_count = 0;
        // run_perft(game_position, 4);
        // cout << "Call count:              " << call_count << endl;
        //cout << sizeof(Position) << " bytes" << endl;
        //run_perft_tests();

    }
    else {
        seconds_per_move = 5;
        uci_loop(game_position);
    }

    
}