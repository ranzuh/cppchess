#include <iostream>
#include <math.h>
#include "position.h"
#include "linear_evaluation.h"
#include "dvector.h"

using namespace std;

extern dvector weights;

void train(vector<Position> leaf_positions, int result, int side) {
    // setup stepsize alpha and vector delta to hold the changes to weights
    double alpha = 1;
    double lambda = 0.7;
    dvector delta(380, 0.0);
    dvector trace(380, 0.0);

    // loop through PV leaf positions
    for(int i = 0; i < leaf_positions.size(); i++) {
        // extract features of leaf pos, this is the gradient of eval w.r.t. weights
        dvector features = extract_features(leaf_positions[i]);

        // eval current state with features
        double eval_state = evaluate_features(leaf_positions[i], features);
        double tanh_eval_state = tanh(0.00255*eval_state);

        // eval next state. If its terminal its eval is the reward or matescore etc.
        double eval_next = 0.0;
        double tanh_eval_next = 0.0;
        if (i == leaf_positions.size()-1) {
            tanh_eval_next = result;
            // eval_next = eval_state;
        }
        else {
            dvector features_next = extract_features(leaf_positions[i+1]);
            eval_next = evaluate_features(leaf_positions[i+1], features_next);
            tanh_eval_next = tanh(0.00255*eval_next);
        }
        cout << "eval " << eval_state << endl;
        cout << "evalnext " << eval_next << endl;
        cout << "tanh eval " << tanh_eval_state << endl;
        cout << "tanh evalnext " << tanh_eval_next << endl;

        // increment eligibility trace
        dvector gradient(380, 0.0);
        // nabla_w tanh(b*V) = (1-tanh^2(b*V)) * b*x
        gradient = vector_scale(vector_scale(features, 0.00255), 1 - pow(tanh_eval_state, 2.0));
        trace = vector_add(vector_scale(trace, lambda), gradient);
        //cout << "trace" << endl;
        //print_vector(trace);

        // calculate temporal difference
        double td = tanh_eval_next - tanh_eval_state;
        cout << "td " << td << endl;

        // add trace scaled by temporal difference to weight vector delta
        delta = vector_add(delta, vector_scale(trace, td));
    }

    // scale delta with stepsize alpha
    delta = vector_scale(delta,  alpha);
    cout << "delta" << endl;
    print_vector(delta);

    // add delta to current weights
    weights = vector_add(weights, delta);
}
