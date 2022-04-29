#include <iostream>
#include <math.h>
#include "position.h"
#include "linear_evaluation.h"
#include "dvector.h"

using namespace std;

extern dvector weights;

void train(vector<Position> leaf_positions, int result, int side) {
    // setup stepsize alpha and vector delta to hold the changes to weights
    double alpha = 0.01;
    dvector delta(380, 0.0);

    // loop through PV leaf positions
    for(int i = 0; i < leaf_positions.size(); i++) {
        // extract features of leaf pos, this is the gradient of eval w.r.t. weights
        dvector features = extract_features(leaf_positions[i]);

        // eval current state with features
        double eval_state = evaluate_features(leaf_positions[i], features, side);

        // eval next state. If its terminal its eval is the reward or matescore etc.
        double eval_next = 0.0;
        if (i == leaf_positions.size()-1) {
            eval_next = result * 2000;
        }
        else {
            eval_next = linear_evaluate_position(leaf_positions[i+1], side);
        }
        cout << "eval " << eval_state << endl;
        cout << "evalnext " << eval_next << endl;

        // calculate temporal difference
        double td = eval_next - eval_state;
        cout << "td " << td << endl;

        // add gradient scaled by temporal difference to weight delta
        delta = vector_add(delta, vector_scale(features, td));
    }

    // scale delta with stepsize alpha
    delta = vector_scale(delta,  alpha);
    cout << "delta" << endl;
    print_vector(delta);

    // add delta to current weights
    weights = vector_add(weights, delta);
}
