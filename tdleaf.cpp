#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include "position.h"
#include "linear_evaluation.h"

using namespace std;

typedef vector<double> dvector;

extern dvector weights;

dvector vector_add(dvector a, dvector b) {
    std::transform(a.begin(), a.end(), b.begin(),
        a.begin(), std::plus<double>());
    return a;
}

dvector vector_multiply(dvector a, dvector b) {
    std::transform(a.begin(), a.end(), b.begin(),
        a.begin(), std::multiplies<double>());
    return a;
}

void update_weights() {
    dvector diff(380, 0.1);
    weights = vector_add(weights, diff);
}

void print_vector(dvector vector) {
    for(auto i : vector) {
        cout << i << ", ";
    }
    cout << endl;
}

void train(vector<Position> leaf_positions, int result) {
    dvector delta(380, 0.0);
    // vector because im lazy to implement scalar product
    dvector alpha(380, 0.1);
    dvector features(380, 0.0);
    

    for(int i = 0; i < leaf_positions.size(); i++) {
        extract_features(leaf_positions[i], features);
        double eval_state = evaluate_features(leaf_positions[i], features);
        double eval_next = 0.0;
        if (i == leaf_positions.size()-1) {
            eval_next = result * 20000;
        }
        else {
            eval_next = linear_evaluate_position(leaf_positions[i+1]);
        }
        cout << "eval " << eval_state << endl;
        cout << "evalnext " << eval_next << endl;

        dvector td(380, eval_next-eval_state);
        cout << "td " << td[0] << endl;
        delta = vector_add(delta, vector_multiply(alpha, vector_multiply(features, td)));
    }
    cout << "delta" << endl;
    print_vector(delta);
}
