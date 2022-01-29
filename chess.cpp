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

enum attacker_pieces : int { ap = 0x1, aP = 0x2, aN = 0x4, aB = 0x8, aR = 0x10, aQ = 0x20, aK = 0x40 };
int piece_to_attacker[] = { 0, ap, aN, aB, aR, aQ, aK, aP, aN, aB, aR, aQ, aK };
int attack_table[240] {
	//  0,    0,    0,    0,    0,    0,    0,    0,
	 0x28,    0,    0,    0,    0,    0,    0, 0x30,
	    0,    0,    0,    0,    0,    0, 0x28,    0,
		0, 0x28,    0,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0,    0, 0x28,    0,    0,
		0,    0, 0x28,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0, 0x28,    0,    0,    0,
		0,    0,    0, 0x28,    0,    0,    0, 0x30,
		0,    0,    0, 0x28,    0,    0,    0,    0,
		0,    0,    0,    0, 0x28,    0,    0, 0x30,
		0,    0, 0x28,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0, 0x28,  0x4, 0x30,
	  0x4, 0x28,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,  0x4, 0x69, 0x70,
	 0x69,  0x4,    0,    0,    0,    0,    0,    0,
	 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x70,    0,
	 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,    0,
		0,    0,    0,    0,    0,  0x4, 0x6A, 0x70,
	 0x6A,  0x4,    0,    0,    0,    0,    0,    0,   
		0,    0,    0,    0,    0, 0x28,  0x4, 0x30,
	  0x4, 0x28,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0, 0x28,    0,    0, 0x30,
		0,    0, 0x28,    0,    0,    0,    0,    0,
		0,    0,    0, 0x28,    0,    0,    0, 0x30,
		0,    0,    0, 0x28,    0,    0,    0,    0,
		0,    0, 0x28,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0, 0x28,    0,    0,    0,   
		0, 0x28,    0,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0,    0, 0x28,    0,    0,
	 0x28,    0,    0,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0,    0,    0, 0x28,    0,
};

// calculate difference of squares for attack table
inline int square_diff(int attacker_sq, int defender_sq) {
    return attacker_sq - defender_sq + 0x77;
}

// is piece-type attacking (potentially) defender square from attacker square
inline int square_attacked_by(int piece, int attacker_sq, int defender_sq) {
	return (piece_to_attacker[piece] & attack_table[square_diff(attacker_sq, defender_sq)]) != 0;
}

int is_square_attacked2(Position &pos, int square, int side) {
    return 0;
}

int main() {
    init_random_keys();
    Position game_position;
    clear_hash_table();

    int debug = 1;

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

        //cout <<  (h8 - a1) + 0x77 << endl;
		// e, p, n, b, r, q, k, P, N, B, R, Q, K, o



        //cout << square_attacked_by(p, b4, c3) << endl;


        // string s = "";
        // for (int i = 0; i < pv_length[0]; i++) {
        //     assert(parse_move(game_position, get_move_string(pv_table[0][i])) != 0);
        //     game_position.make_move(pv_table[0][i]);
        // }

        // call_count = 0;
        //run_perft(game_position, 5);
        // cout << "Call count:              " << call_count << endl;
        //cout << sizeof(Position) << " bytes" << endl;
        //run_perft_tests();

    }
    else {
        seconds_per_move = 5;
        uci_loop(game_position);
    }

    
}