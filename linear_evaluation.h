#ifndef LINEAR_EVALUATION_H
#define LINEAR_EVALUATION_H

//#include <vector>
#include "dvector.h"

double linear_evaluate_position(Position &pos, int side);
dvector extract_features(Position &pos);
double evaluate_features(Position &pos, dvector &features);
void init_weights_from_json();
void save_weights_to_json();
void print_features(dvector &features);

#endif