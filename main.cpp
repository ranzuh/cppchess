#include <iostream>
#include <chrono>
#include <vector>
#include "position.h"
#include "hashtable.h"
#include "uci.h"
#include "search.h"
#include "perft.h"
#include "linear_evaluation.h"
#include "tdleaf.h"

using namespace std;

string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
string tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

int main() {
    init_random_keys();
    Position game_position;
    clear_hash_table();

    int debug = 0;

    if (debug) {
        game_position.parse_fen(start_position);
        // game_position.print_board();
        // game_position.print_board_stats();
        // parse_position(game_position, "position fen 8/6k1/5p2/6p1/2P4p/P2Q3P/2P2PP1/4q1K1 w - - 3 41");
        // parse_position(game_position, "position fen 8/6k1/5p2/6p1/2P4p/P2Q3P/2P2PP1/4q1K1 w - - 3 41 moves d3f1 e1c3");
        // game_position.parse_fen(start_position);

        // game_position.print_board();
        // game_position.print_board_stats();

        // search_position(game_position, 20);
        // cout << "Quiescence search nodes: " << quiesc_nodes << endl;
        // cout << "Table hits:              " << table_hits << endl;
		
        //init_weights_from_json();
        //extern double weights[380];


        // parse_position(game_position, "moves e2e4 b8c6 d2d4 g8f6 b1c3 d7d5 e4e5 c8g4 g1e2");
        // game_position.print_board();
        // cout << linear_evaluate_position(game_position, white) << endl;

        run_perft_tests();

        

        // weights[0] = 101;
        // save_weights_to_json();

		//run_perft(game_position, 1);

		// game_position.make_move(encode_move(e2, a6, 0, 1, 0, 0, 0), 10);
		// game_position.print_board();
        // game_position.print_board_stats();

		// unmake_move(game_position, encode_move(e2, a6, 0, 1, 0, 0, 0), 10);
		// game_position.print_board();
        // game_position.print_board_stats();


    }
    else {
        seconds_per_move = 2;
        uci_loop();
    }

    
}