#include <chrono>
#include <cassert>
#include <cstring>
#include "position.h"
#include "movegen.h"
#include "hashtable.h"
#include "evaluation.h"

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
    generate_pseudo_moves(pos, moves);

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
    generate_pseudo_moves(pos, moves);

    // if we are following pv line
    if (follow_pv) {
        // enable pv move scoring
        enable_pv_scoring(moves);
    }

    // sort moves
    sort_moves(pos, moves);

    // copy board state
    Position copy = pos;

    int legal_moves = 0;

    for (int i = 0; i < moves.count; i++) {
        // if (leftmost) {
        //     print_move_scores(pos, moves);
        //     cout << endl;
        // }

        if (pos.make_move(moves.moves[i])) {
            legal_moves++;
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

    // if no legal moves
    if (legal_moves == 0) {
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

        if (!stopped) {
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
        
    }
    cout << "bestmove " << get_move_string(pv_table[0][0]) << endl; 
}