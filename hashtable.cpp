#include <iostream>
#include "position.h"
#include "hashtable.h"
#include "search.h"

using namespace std;

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
            int score = hash_entry->score;
            
            if (score < -mate_score) score += ply;
            if (score > mate_score) score -= ply;
            
            if (hash_entry->flag == hash_flag_exact)
                return score;
            if (hash_entry->flag == hash_flag_alpha)
                if (score <= alpha)
                    return alpha;
            if (hash_entry->flag == hash_flag_beta)
                if (score >= beta)
                    return beta;
        }
    }

    return no_hash_entry;
}

void write_hash_entry(uint64_t hash_key, int score, int depth, int hash_flag) {
    tt *hash_entry = &hash_table[hash_key % hash_table_size];

    if (score < -mate_score) score -= ply;
    if (score > mate_score) score += ply;

    hash_entry->key = hash_key;
    hash_entry->score = score;
    hash_entry->depth = depth;
    hash_entry->flag = hash_flag;
    
}