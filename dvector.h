#ifndef DVECTOR_H
#define DVECTOR_H

#include <vector>

typedef std::vector<double> dvector;

dvector vector_add(dvector a, dvector b);
dvector vector_multiply(dvector a, dvector b);
dvector vector_scale(dvector a, double scalar);
void print_vector(dvector vector);

#endif