#ifndef SEARCH_H
#define SEARCH_H

#define infinity 500000
#define mate_value 49000
#define mate_score 48000

extern int pv_length[];
extern int pv_table[][64];
extern uint64_t nodes;
extern uint64_t quiesc_nodes;
extern int table_hits;
extern int ply;

int search_position(Position &pos, int depth, bool print=true);


#endif