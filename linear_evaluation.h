#ifndef LINEAR_EVALUATION_H
#define LINEAR_EVALUATION_H

double linear_evaluate_position(Position &pos);
void extract_features(Position &pos, vector<double> &features);
double evaluate_features(Position &pos, vector<double> &features);
void init_weights_from_json();
void save_weights_to_json();
void print_features(int features[]);

#endif