#include <vector>
#include <algorithm>
#include <iostream>
#include <functional>

typedef std::vector<double> dvector;

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

dvector vector_scale(dvector a, double scalar) {
    for (int i = 0; i < a.size(); i++) {
        a[i] *= scalar;
    }
    return a;
}

void print_vector(dvector vector) {
    for(auto i : vector) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}