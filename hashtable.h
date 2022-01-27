#ifndef HASHTABLE_H
#define HASHTABLE_H

// define constants for hash func

#define hash_flag_exact  0
#define hash_flag_alpha  1
#define hash_flag_beta   2
#define no_hash_entry    1000000 // outside alpha beta limits

int read_hash_entry(uint64_t hash_key, int alpha, int beta, int depth);
void write_hash_entry(uint64_t hash_key, int score, int depth, int hash_flag);
void init_random_keys();
void clear_hash_table();

#endif