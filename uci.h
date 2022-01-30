#ifndef UCI_H
#define UCI_H

// globals for timing
extern int stopped;
extern int seconds_per_move;
extern chrono::steady_clock::time_point go_start;

void uci_loop();
int parse_move(Position &pos, string move_string);

#endif