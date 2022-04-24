#ifndef LINEAR_EVALUATION_H
#define LINEAR_EVALUATION_H

int linear_evaluate_position(Position &pos);
void extract_features(Position &pos, int features[]);
double evaluate_features(Position &pos, int features[]);
void init_weights_from_json();
void save_weights_to_json();

#endif