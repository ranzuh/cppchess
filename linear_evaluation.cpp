#include <numeric>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "position.h"
#include "dvector.h"

// number of features == number of weights
const int num_features = 380;

typedef vector<double> dvector; 

dvector weights {
100, 350, 350, 525, 1000,

//0,  0,  0,  0,  0,  0,  0,  0,
50, 50, 50, 50, 50, 50, 50, 50,
10, 10, 20, 30, 30, 20, 10, 10,
5,  5, 10, 25, 25, 10,  5,  5,
0,  0,  0, 20, 20,  0,  0,  0,
5, -5,-10,  0,  0,-10, -5,  5,
5, 10, 10,-20,-20, 10, 10,  5,
//0,  0,  0,  0,  0,  0,  0,  0,

-50,-40,-30,-30,-30,-30,-40,-50,
-40,-20,  0,  0,  0,  0,-20,-40,
-30,  0, 10, 15, 15, 10,  0,-30,
-30,  5, 15, 20, 20, 15,  5,-30,
-30,  0, 15, 20, 20, 15,  0,-30,
-30,  5, 10, 15, 15, 10,  5,-30,
-40,-20,  0,  5,  5,  0,-20,-40,
-50,-40,-30,-30,-30,-30,-40,-50,

-20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20,

0,  0,  0,  0,  0,  0,  0,  0,
5, 10, 10, 10, 10, 10, 10,  5,
-5,  0,  0,  0,  0,  0,  0, -5,
-5,  0,  0,  0,  0,  0,  0, -5,
-5,  0,  0,  0,  0,  0,  0, -5,
-5,  0,  0,  0,  0,  0,  0, -5,
-5,  0,  0,  0,  0,  0,  0, -5,
0,  0,  0,  5,  5,  0,  0,  0,

-20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
-5,  0,  5,  5,  5,  5,  0, -5,
0,  0,  5,  5,  5,  5,  0, -5,
-10,  5,  5,  5,  5,  5,  0,-10,
-10,  0,  5,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20,

0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  5,  5,  5,  5,  0,  0,
0,  5,  5, 10, 10,  5,  5,  0,
0,  5, 10, 20, 20, 10,  5,  0,
0,  5, 10, 20, 20, 10,  5,  0,
0,  0,  5, 10, 10,  5,  0,  0,
0,  5,  5, -5, -5,  0,  5,  0,
0,  0,  5,  0,-15,  0,  10, 0,

-10, -20, -8, 20, 10, 15, 20,
};

// from evaluation.cpp
extern int mirror_square[];
extern int pawn_rank[2][10];

void init_weights_from_json() {
    // read a JSON file
    std::ifstream i("weights.json");
    nlohmann::json j;
    i >> j;
    // copy weights from json object to weights array
    std::copy(j["weights"].begin(), j["weights"].end(), weights.begin());
}

void save_weights_to_json() {
    nlohmann::json j;
    j["weights"] = weights;
    // write JSON to weights.json
    std::ofstream o("weights.json");
    o << std::setw(4) << j << std::endl;
}

// map square in 0..127 -> 0..63
inline int get_square_in_64(int square) {
    return (square >> 4) * 8 + (square & 7);
}

// evaluate white pawn structures
void eval_white_pawn(int square, int rank, int file, dvector &arr) {
    //int score = 0;

    // increment file (remember I used 0..9 files)
    file += 1;

    // if this pawn is not the least advanced one, its doubled
	if (pawn_rank[white][file] > rank) {
        //score -= doubled_pawn_penalty;
        arr[373]++;
    }

	// if no friendly pawns on adjacent files its isolated
	if ((pawn_rank[white][file - 1] == 0) && (pawn_rank[white][file + 1] == 0)) {
        //score -= isolated_pawn_penalty;
        arr[374]++;
    }

	// if pawn not isolated it might be backwards
	else if ((pawn_rank[white][file - 1] < rank) && (pawn_rank[white][file + 1] < rank)) {
		//score -= backwards_pawn_penalty;
        arr[375]++;
    }

	// give bonus if pawn is passed
	if ((pawn_rank[black][file - 1] >= rank) &&
		    (pawn_rank[black][file] >= rank) &&
		    (pawn_rank[black][file + 1] >= rank)) {
		//score += (7 - rank) * passed_pawn_bonus;
        arr[376] += (7 - rank);
    }

	//return score;
}

// evaluate white pawn structures
void eval_black_pawn(int square, int rank, int file, dvector &arr) {
    //int score = 0;

    // increment file (remember I used 0..9 files)
    file += 1;

    // if this pawn is not the least advanced one, its doubled
	if (pawn_rank[black][file] < rank) {
		//score -= doubled_pawn_penalty;
        arr[373]--;
    }
	// if no friendly pawns on adjacent files its isolated
	if ((pawn_rank[black][file - 1] == 7) && (pawn_rank[black][file + 1] == 7)) {
		//score -= isolated_pawn_penalty;
        arr[374]--;
    }
	// if pawn not isolated it might be backwards
	else if ((pawn_rank[black][file - 1] > rank) && (pawn_rank[black][file + 1] > rank)) {
		//score -= backwards_pawn_penalty;
        arr[375]--;
    }
	// give bonus if pawn is passed
	if ((pawn_rank[white][file - 1] <= rank) &&
			(pawn_rank[white][file] <= rank) &&
			(pawn_rank[white][file + 1] <= rank)) {
		//score += rank * passed_pawn_bonus;
        arr[376] -= rank;
    }

	//return score;
}

// evaluate whites rook
void evaluate_white_rook(int rank, int file, dvector &arr) {
    //int score = 0;

    // check if there is black pawn on same file as the rook
    if (pawn_rank[black][file + 1] == 7) {
        // check if there is also white pawn on same file as the rook
        if (pawn_rank[white][file + 1] == 0) {
            //score += rook_open_file_bonus;
            arr[378]++;
        }
        else {
            //score += rook_semi_open_file_bonus;
            arr[377]++;
        }
    }

    // check if rook is on seventh rank
    if (rank == 1) {
        //score += rook_on_seventh_bonus;
        arr[379]++;
    }
    
    //return score;
}

// evaluate blacks rook
void evaluate_black_rook(int rank, int file, dvector &arr) {
    //int score = 0;

    // check if there is white pawn on same file as the rook
    if (pawn_rank[white][file + 1] == 0) {
        // check if there is also black pawn on same file as the rook
        if (pawn_rank[black][file + 1] == 7) {
            //score += rook_open_file_bonus;
            arr[378]--;
        }
        else {
            //score += rook_semi_open_file_bonus;
            arr[377]--;
        }
    }

    // check if rook is on second rank
    if (rank == 6) {
        //score += rook_on_seventh_bonus;
        arr[379]--;
    }
    
    //return score;
}

/* evaluate position based on material, piece-square tables and
   basic pawn structures */
dvector extract_features(Position &pos) {
	int file, rank;
	//int score = 0;
    int square;
    int piece;

    dvector features(380, 0.0);

	/* first store least advanced pawns on every file */

    // init each file to max advancement for each side
	for (int i = 0; i < 10; i++) {
		pawn_rank[white][i] = 0;
		pawn_rank[black][i] = 7;
	}

    // store the pawns
    for (rank = 1; rank < 7; rank++) {
        for (file = 0; file < 8; file++) {
            square = rank * 16 + file;
            piece = pos.board[square];
            if (piece == e)
                continue;

            if (piece == P && pawn_rank[white][file + 1] < rank) 
                pawn_rank[white][file + 1] = rank;

            else if (piece == p && pawn_rank[black][file + 1] > rank)
                pawn_rank[black][file + 1] = rank;
        }
	}

    /* evaluate each pawn or piece */

    for (rank = 0; rank < 8; rank++) {
        for (file = 0; file < 8; file++) {
            // square as 0..127
            square = rank * 16 + file;

            if (pos.board[square] != e) {
                piece = pos.board[square];
                // sum the material
                //score += piece_values[piece];
                switch (piece) {
                case P:
                    //score += pawn_table[get_square_in_64(square)];
                    eval_white_pawn(square, rank, file, features);
                    //cout << score << endl;
                    features[0]++;
                    // +5 because of material, -8 because of 8th row not included
                    features[5 + get_square_in_64(square) - 8]++;
                    break;
                case N:
                    //score += knight_table[get_square_in_64(square)];
                    features[1]++;
                    // 5 + 48 = 53
                    features[53 + get_square_in_64(square)]++;
                    break;
                case B:
                    //score += bishop_table[get_square_in_64(square)];
                    features[2]++;
                    features[117 + get_square_in_64(square)]++;
                    break;
                case R:
                    //score += rook_table[get_square_in_64(square)];
                    evaluate_white_rook(rank, file, features);
                    features[3]++;
                    features[181 + get_square_in_64(square)]++;
                    break;
                case Q:
                    //score += queen_table[get_square_in_64(square)];
                    features[4]++;
                    features[245 + get_square_in_64(square)]++;
                    break;
                case K:
                    //score += king_table[get_square_in_64(square)];
                    features[309 + get_square_in_64(square)]++;
                    break;
                case p:
                    //score -= pawn_table[mirror_square[get_square_in_64(square)]];
                    eval_black_pawn(square, rank, file, features);
                    //cout << score << endl;
                    features[0]--;
                    // +5 because of material, -8 because of 8th row not included
                    features[5 + mirror_square[get_square_in_64(square)] - 8]--;
                    break;
                case n:
                    //score -= knight_table[mirror_square[get_square_in_64(square)]];
                    features[1]--;
                    // 5 + 48 = 53
                    features[53 + mirror_square[get_square_in_64(square)]]--;
                    break;
                case b:
                    //score -= bishop_table[mirror_square[get_square_in_64(square)]];
                    features[2]--;
                    features[117 + mirror_square[get_square_in_64(square)]]--;
                    break;
                case r:
                    //score -= rook_table[mirror_square[get_square_in_64(square)]];
                    evaluate_black_rook(rank, file, features);
                    features[3]--;
                    features[181 + mirror_square[get_square_in_64(square)]]--;
                    break;
                case q:
                    //score -= queen_table[mirror_square[get_square_in_64(square)]];
                    features[4]--;
                    features[245 + mirror_square[get_square_in_64(square)]]--;
                    break;
                case k:
                    //score -= king_table[mirror_square[get_square_in_64(square)]];
                    features[309 + mirror_square[get_square_in_64(square)]]--;
                    break;
                
                default:
                    break;
                }
            }
        }
    }
    return features;
}

void print_features(dvector &features) {
    string names[] = {
                "material",
                "pawn table",
                "knight table",
                "bishop table",
                "rook table",
                "queen table",
                "king table",
                "pawn/rook structures",
            };

    int j = 0;
    // loop through features
    for (int i = 0; i < 380; i++)
    {

        if (i == 0 || i == 5 || i == 53) {
            std::cout << names[j] << endl;
            j++;
        }
        if (i > 53 && (i-5+16) % 64 == 0) {
            std::cout << names[j] << endl;
            j++;
        }
        std::cout << features[i] << " ";
        if (i == 4) std::cout << std::endl;
        if (i > 4 && (i+4) % 8 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;

}

double evaluate_features(Position &pos, dvector &features) {
    double score = std::inner_product(features.begin(), features.end(), weights.begin(), 0.0);
    //print_features(features);
    //pos.print_board();
    //pos.print_board_stats();
    return score;
}

double linear_evaluate_position(Position &pos, int side) {
    dvector features = extract_features(pos);
    double score = evaluate_features(pos, features);
    return side == white ? score : -score;
}


