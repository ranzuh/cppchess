#ifndef LINEAR_EVALUATION_H
#define LINEAR_EVALUATION_H

int linear_evaluate_position(Position &pos);
void extract_features(Position &pos, int features[]);
int evaluate_features(Position &pos, int features[]);

#endif