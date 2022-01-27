#ifndef SEARCH_H
#define SEARCH_H

extern int pv_length[];
extern int pv_table[][64];
extern int nodes;
extern int quiesc_nodes;
extern int table_hits;

void search_position(Position &pos, int depth);


#endif