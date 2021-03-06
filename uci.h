#ifndef UCI_H
#define UCI_H

// globals for timing
extern int stopped;
extern int seconds_per_move;
extern chrono::steady_clock::time_point go_start;
extern int time_set;
extern chrono::steady_clock::time_point stop_time;
extern int time_left;

void uci_loop();
int parse_move(Position &pos, string move_string);
int parse_go(Position &pos, string command, bool print=true);
void parse_position(Position &pos, string command);

void check_time();

#endif