#include <iostream>
#include <chrono>
#include "position.h"
#include "hashtable.h"
#include "uci.h"
#include "search.h"
#include "perft.h"
#include "linear_evaluation.h"

using namespace std;

string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
string tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

int main() {
    init_random_keys();
    Position game_position;
    clear_hash_table();

    int debug = 1;

    if (debug) {
        game_position.parse_fen(start_position);
        // game_position.print_board();
        // game_position.print_board_stats();
        // parse_position(game_position, "position fen 8/6k1/5p2/6p1/2P4p/P2Q3P/2P2PP1/4q1K1 w - - 3 41");
        // parse_position(game_position, "position fen 8/6k1/5p2/6p1/2P4p/P2Q3P/2P2PP1/4q1K1 w - - 3 41 moves d3f1 e1c3");
        // game_position.parse_fen(start_position);

        // game_position.print_board();
        // game_position.print_board_stats();

        // time_set = 1;
        // auto start_time = chrono::steady_clock::now();
        // auto move_time_chrono = chrono::milliseconds(15000);
        // stop_time = start_time + move_time_chrono;

        // search_position(game_position, 20);
        // cout << "Quiescence search nodes: " << quiesc_nodes << endl;
        // cout << "Table hits:              " << table_hits << endl;
		
        init_weights_from_json();
        extern double weights[380];

        Position pos1;
        parse_position(pos1, "position moves e2e4 b8c6");
        Position pos2;
        parse_position(pos2, "position moves e2e4 b8c6 d2d4 g8f6 b1c3 d7d5 e4e5 f6e4 g1e2 c8f5");

        double eval2 = linear_evaluate_position(pos2);

        int features[380] = {0};
        extract_features(pos1, features);

        double eval1 = evaluate_features(pos1, features);

        cout << "eval1 " << eval1 << endl;
        cout << "eval2 " << eval2 << endl;

        double diff = eval2 - eval1;

        cout << "diff " << diff << endl;

        print_features(features);


        // weights[0] = 101;
        // save_weights_to_json();



        // int asd = 1;

        // int features[380] = {0};
        // extract_features(game_position, features);
        // double score = evaluate_features(game_position, features);
        // std::cout << "eval " << score << std::endl;
        // std::cout.precision(std::numeric_limits<double>::max_digits10 - 1);
        // std::cout << std::scientific << asd * 1.01 << '\n';
		// run_perft_tests();
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