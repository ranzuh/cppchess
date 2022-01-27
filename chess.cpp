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

/////////////////////////////////////
// TRANSPOSITION TABLE AND HASHING //
/////////////////////////////////////

// random piece keys [piece][square]
uint64_t piece_keys[13][64];

// random enpassant keys [square]
uint64_t enpassant_keys[64];

// random castling keys
uint64_t castle_keys[16];

// random side key
uint64_t side_key;

// pseudo random number state

unsigned int state = 1804289383;

// generate 32-bit pseudo random numbers
unsigned int get_random_U32_number() {
    // get current state
    unsigned int number = state;

    // XOR Shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    // update random number state
    state = number;

    return state;
}

// generate 64-bit pseudo random numbers
uint64_t get_random_U64_number() {
    // init 4 random numbers
    uint64_t n1, n2, n3, n4;

    n1 = (uint64_t)(get_random_U32_number() & 0xffff);
    n2 = (uint64_t)(get_random_U32_number() & 0xffff);
    n3 = (uint64_t)(get_random_U32_number() & 0xffff);
    n4 = (uint64_t)(get_random_U32_number() & 0xffff);

    // return random number
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

void init_random_keys() {
    // loop over piece codes
    for (int piece = p; piece <= K; piece++) {
        // loop over squares
        for (int square = 0; square < 64; square++) {
            // init random piece keys
            piece_keys[piece][square] = get_random_U64_number();
        }
    }

    // loop over squares
    for (int square = 0; square < 64; square++) {
        // init random enpassant keys
        enpassant_keys[square] = get_random_U64_number();
    }

    // init random side key
    side_key = get_random_U64_number();

    // loop over castling keys
    for (int index = 0; index < 16; index++) {
        // init castle random keys
        castle_keys[index] = get_random_U64_number();
    } 
}

// generate "almost unique" position ID aka hash key from scratch
uint64_t generate_hash_key(Position &pos) {
    // final hash key
    uint64_t final_key = 0;

    // loop over squares
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            // square as 0..127
            int square = rank * 16 + file;
            // square as 0..63
            int square_in_64 = rank * 8 + file;

            int piece = pos.board[square];

            if (!(square & 0x88) && piece != e) {
                final_key ^= piece_keys[piece][square_in_64];
            }
        }
    }

    // if enpassant square is on board
    if (pos.enpassant != no_sq) {
        // hash enpassant
        int square = pos.enpassant;
        int square_in_64 = (square >> 4) * 8 + (square & 7);
        final_key ^= enpassant_keys[square_in_64];
    }

    // hash castling rights
    final_key ^= castle_keys[pos.castle];

    // hash side to move only on black to move
    if (pos.side == black) {
        final_key ^= side_key;
    }

    // return final key
    return final_key;
}

// define constants for hash func

#define hash_flag_exact  0
#define hash_flag_alpha  1
#define hash_flag_beta   2
#define no_hash_entry    1000000 // outside alpha beta limits

struct tt {
    uint64_t key;   // hash key
    int depth;      // search depth
    int flag;       // type of node (pv, alpha, or beta)
    int score;      // score (exact, alpha, or beta)
    //int best_move;
};

// init hashtable size to 8 MB
const int hash_table_size = 8 * 1024 * 1024 / sizeof(tt);

tt hash_table[hash_table_size];

void clear_hash_table() {
    for (tt &entry : hash_table) {
        entry.key = 0;
        entry.depth = 0;
        entry.flag = 0;
        entry.score = 0;
    }
}

int read_hash_entry(uint64_t hash_key, int alpha, int beta, int depth) {
    tt *hash_entry = &hash_table[hash_key % hash_table_size];

    if (hash_entry->key == hash_key) {
        if (hash_entry->depth >= depth) {
            if (hash_entry->flag == hash_flag_exact)
                return hash_entry->score;
            if (hash_entry->flag == hash_flag_alpha)
                if (hash_entry->score <= alpha)
                    return alpha;
            if (hash_entry->flag == hash_flag_beta)
                if (hash_entry->score >= beta)
                    return beta;
        }
    }

    return no_hash_entry;
}

void write_hash_entry(uint64_t hash_key, int score, int depth, int hash_flag) {
    tt *hash_entry = &hash_table[hash_key % hash_table_size];

    hash_entry->key = hash_key;
    hash_entry->score = score;
    hash_entry->depth = depth;
    hash_entry->flag = hash_flag;
    
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

/*
    Most valuable victim & less valuable attacker
                          
          (Victims) Pawn Knight Bishop   Rook  Queen   King
 (Attackers)
        Pawn        105    205    305    405    505    605
      Knight        104    204    304    404    504    604
      Bishop        103    203    303    403    503    603
        Rook        102    202    302    402    502    602
       Queen        101    201    301    401    501    601
        King        100    200    300    400    500    600
*/

// MVV LVA [attacker][victim] eg. P x q
int mvv_lva[13][13] = {
    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,
 	0, 105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	0, 104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	0, 103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	0, 102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	0, 101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	0, 100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	0, 105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	0, 104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	0, 103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	0, 102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	0, 101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	0, 100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,
};

const int max_depth = 64;

/*
      ================================
            Triangular PV table
      --------------------------------
        PV line: e2e4 e7e5 g1f3 b8c6
      ================================
           0    1    2    3    4    5
      
      0    m1   m2   m3   m4   m5   m6
      
      1    0    m2   m3   m4   m5   m6 
      
      2    0    0    m3   m4   m5   m6
      
      3    0    0    0    m4   m5   m6
       
      4    0    0    0    0    m5   m6
      
      5    0    0    0    0    0    m6
*/
// PV length
int pv_length[max_depth];

// PV table
int pv_table[max_depth][max_depth];

// follow PV and score PV move
int follow_pv, score_pv;

// killer moves [id][ply]
int killer_moves[2][max_depth];

// history moves [piece][square]
int history_moves[12][64];

uint64_t nodes = 0;

uint64_t quiesc_nodes = 0;

int ply = 0;

int max_ply = 0;

// enable pv move scoring
void enable_pv_scoring(Movelist &moves) {
    // disable following pv
    follow_pv = 0;

    for (int i = 0; i < moves.count; i++) {
        // make sure we hit pv
        if (pv_table[0][ply] == moves.moves[i]) {
            //enable move scoring
            score_pv = 1;
            // enable following pv
            follow_pv = 1;

        }
    }
}

// score a move for move ordering purposes
int score_move(Position &pos, int move) {

    // if pv scoring enabled
    if (score_pv) {
        // make sure the move is pv move
        if (pv_table[0][ply] == move) {
            // disable move scoring
            score_pv = 0;

            //cout << "Current pv move: " <<  get_move_string(move) << " ply: " << ply << endl;
            // prioritize pv move to sort it at the top
            return 20000;
        }
    }

    // score captures
    if (decode_capture(move)) {
        int source_piece = pos.board[decode_source(move)];
        // if target is empty its en passant move so set target piece to pawn
        int target_piece = pos.board[decode_target(move)] == e ? P : pos.board[decode_target(move)];

        // score capture by MVV-LVA lookup
        // cout << Position::square_to_coord[decode_source(move)] << Position::square_to_coord[decode_target(move)] << endl;
        // cout << "source piece: " << Position::ascii_pieces[source_piece] << endl;
        // cout << "target piece: " << Position::ascii_pieces[target_piece] << endl;
        // cout << "score: " << mvv_lva[source_piece][target_piece] << endl;

        // add 10000 to add priority over quiet moves
        return mvv_lva[source_piece][target_piece] + 10000;
    }
    // score quiet moves
    else {

        // score 1st killer move
        if (killer_moves[0][ply] == move) {
            return 9000;
        }
        // score 2nd killer move
        else if (killer_moves[1][ply] == move) {
            return 8000;
        }
        // score history move
        
    }
    return 0;
}

// prints move scores of moves in movelist
void print_move_scores(Position &pos, Movelist &moves) {
    //enable move scoring
    score_pv = 1;
    for (int i = 0; i < moves.count; i++) {
        cout << "move: " << get_move_string(moves.moves[i]) << " score: " << score_move(pos, moves.moves[i]) << endl;
    }
}

// use struct to pass position to comparison function
struct Compare_move_scores {
    Position pos;
    Compare_move_scores(Position &pos) {
        this->pos = pos;
    }
    // sort moves in descending order
    bool operator () (int move1, int move2) {
        return score_move(this->pos, move1) > score_move(this->pos, move2);
    }
};

// bool compare_move_scores(Position &pos, int move1, int move2) {
//     return score_move(pos, move1) < score_move(pos, move2);
// }

void sort_moves(Position &pos, Movelist &moves) {
    Movelist move_scores;

    for (int i = 0; i < moves.count; i++) {
        move_scores.add_move(score_move(pos, moves.moves[i]));
    }

    int move, move_score, j;
    for (int i = 1; i < moves.count; i++)
    {
        move = moves.moves[i];
        move_score = move_scores.moves[i];
        j = i - 1;
 
        while (j >= 0 && move_scores.moves[j] < move_score)
        {
            moves.moves[j + 1] = moves.moves[j];
            move_scores.moves[j + 1] = move_scores.moves[j];
            j = j - 1;
        }
        moves.moves[j + 1] = move;
        move_scores.moves[j + 1] = move_score;
    }

    // // loop over current move within a move list
    // for (int current_move = 0; current_move < moves.count; current_move++)
    // {
    //     // loop over next move within a move list
    //     for (int next_move = current_move + 1; next_move < moves.count; next_move++)
    //     {
    //         // compare current and next move scores
    //         if (move_scores[current_move] < move_scores[next_move])
    //         {
    //             // swap scores
    //             int temp_score = move_scores[current_move];
    //             move_scores[current_move] = move_scores[next_move];
    //             move_scores[next_move] = temp_score;
                
    //             // swap moves
    //             int temp_move = moves.moves[current_move];
    //             moves.moves[current_move] = moves.moves[next_move];
    //             moves.moves[next_move] = temp_move;
    //         }
    //     }
    // }

    //sort(moves.moves, moves.moves + moves.count, Compare_move_scores(pos));
}

chrono::steady_clock::time_point go_start;
int stopped = 0;
int seconds_per_move = 5;

void check_time() {
    if (nodes % 2047 == 0) {
        auto stop = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - go_start);
        if (duration.count() > (seconds_per_move * 1000)) {
            //cout << "5 sec passed" << endl;
            stopped = 1;
        }
    }
}

int quiescence_search(Position &pos, int alpha, int beta) {

    check_time();

    // get lower bound score
    int stand_pat = evaluate_position(pos);

    // ensure no overflow of arrays depending on max_depth
    if (ply > max_depth - 1) {
        return stand_pat;
    }

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

    // sort moves
    sort_moves(pos, moves);

    for (int i = 0; i < moves.count; i++) {
        if (decode_capture(moves.moves[i])) {
            if (pos.make_move(moves.moves[i])) {
                nodes++;
                quiesc_nodes++;
                ply++;
                // if (max_ply < ply) {
                //     max_ply = ply;
                //     cout << max_ply << " ply reached" << endl;
                // }
                int value = -quiescence_search(pos, -beta, -alpha);
                ply--;
                pos.rep_index -= 1;
                
                // restore board state
                pos = copy;

                if (stopped) return 0;

                if (value >= beta) {
                    return beta;
                }

                if (value > alpha) {
                    alpha = value;
                }
            }
        }
    }

    return alpha;
}

int leftmost = 1;

int table_hits = 0;

// negamax search with alpha-beta pruning
int negamax(Position &pos, int depth, int alpha, int beta) {

    check_time();

    // init PV length
    pv_length[ply] = ply;

    

    // are we in check? if so, we want to search deeper
    int king_attacked = is_square_attacked(pos, pos.king_squares[pos.side], !pos.side);
	if (king_attacked) {
        depth += 1;
    }

    int hash_flag = hash_flag_alpha;
    int value = read_hash_entry(pos.hash_key, alpha, beta, depth);
    if (ply && value != no_hash_entry) {
        table_hits++;
        return value;
    }

    // ensure no overflow of arrays depending on max_depth
    if (ply > max_depth - 1) {
        value = evaluate_position(pos);
        //write_hash_entry(pos.hash_key, value, depth, hash_flag_exact);
        return value;
    }

    // check repetitions
    if (ply) {
        for (int i = 0; i < pos.rep_index; i++) {
            if (pos.rep_stack[i] == pos.hash_key)
                return 0;
        }
    }

    if (depth == 0) {
        value = quiescence_search(pos, alpha, beta);
        //write_hash_entry(pos.hash_key, value, depth, hash_flag_exact);
        return value;
        //return evaluate_position(pos);
    }
    value = -500000;

    Movelist moves;
    generate_legal_moves(pos, moves);

    // if no legal moves
    if (moves.count == 0) {
        // king is in check - checkmate
        if (king_attacked) {
            //cout << "found mate at ply " << ply << endl;
            value = -49000 + ply;
            write_hash_entry(pos.hash_key, value, depth, hash_flag_exact);
            return value;
        }
        // king is not in check - stalemate
        else {
            //cout << "found stalemate at ply " << ply << endl;
            value = 0;
            write_hash_entry(pos.hash_key, value, depth, hash_flag_exact);
            return value;
        }
    }

    // if we are following pv line
    if (follow_pv) {
        // enable pv move scoring
        enable_pv_scoring(moves);
    }

    // sort moves
    sort_moves(pos, moves);

    // copy board state
    Position copy = pos;

    for (int i = 0; i < moves.count; i++) {
        // if (leftmost) {
        //     print_move_scores(pos, moves);
        //     cout << endl;
        // }

        if (pos.make_move(moves.moves[i])) {
            nodes++;

            ply++;
            value = -negamax(pos, depth - 1, -beta, -alpha);
            leftmost = 0;
            ply--;
            pos.rep_index -= 1;
            
            // restore board state
            pos = copy;

            if (stopped) return 0;
            
            // fail hard beta cutoff
            if (value >= beta) {
                // for quiet moves
                if (decode_capture(moves.moves[i]) == 0) {
                    // store killer moves
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = moves.moves[i];
                }
                write_hash_entry(pos.hash_key, beta, depth, hash_flag_beta);
                
                return beta;
            }
            
            if (value > alpha) {
                alpha = value;
                // add pv move to pv table
                pv_table[ply][ply] = moves.moves[i];

                // loop over the next ply
                for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                    // copy move from deeper ply into a current ply's line
                    pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
                
                // increment pv length for current ply
                pv_length[ply] = pv_length[ply + 1];

                hash_flag = hash_flag_exact;
            }
        }
    }
    
    write_hash_entry(pos.hash_key, alpha, depth, hash_flag);
    return alpha;
}

// search position for the best move
void search_position(Position &pos, int depth) {
    // clear hash table, good idea or not?
    //clear_hash_table();

    int score = 0;

    // reset nodes counters
    table_hits = 0;
    nodes = 0;
    quiesc_nodes = 0;

    follow_pv = 0;
    score_pv = 0;

    // clear helper data structures for search
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));

    // start timing
    auto start = chrono::high_resolution_clock::now();

    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        
        if (stopped) break;

        follow_pv = 1;
        leftmost = 1;
        // find best move within a given position
        score = negamax(pos, current_depth, -500000, 500000);
        // stop clock
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

        // info depth 2 score cp 214 time 1242 nodes 2124 nps 34928 pv e2e4 e7e5
        cout << "info depth " << current_depth << " score cp " << score << " nodes " << nodes << " nps " << uint64_t(1000000.0 * nodes / duration.count()) << " pv ";
        for (int i = 0; i < pv_length[0]; i++) {
            cout << get_move_string(pv_table[0][i]) << " " << flush;
            assert(get_move_string(pv_table[0][i]) != "a8a8");
        }
        cout << endl;
    }
    cout << "bestmove " << get_move_string(pv_table[0][0]) << endl; 
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

int main() {
    init_random_keys();
    Position game_position;
    clear_hash_table();

    //run_perft(game_position, 6);

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

        go_start = chrono::steady_clock::now();

        search_position(game_position, 10);
        cout << "Quiescence search nodes: " << quiesc_nodes << endl;
        cout << "Table hits:              " << table_hits << endl;

        string s = "";
        for (int i = 0; i < pv_length[0]; i++) {
            assert(parse_move(game_position, get_move_string(pv_table[0][i])) != 0);
            game_position.make_move(pv_table[0][i]);
        }


    }
    else {
        uci_loop(game_position);
    }

    
}