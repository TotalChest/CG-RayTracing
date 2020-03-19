#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <cmath>
#include <limits>
#include <vector>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <omp.h>



int threads = 8;
extern const int HEIGHT = 1024;
extern const int WIDTH  = 1024;
extern const float PI = 3.141592;
extern const float EPSILON = 0.00001;
extern const int RECURSION_DEPTH = 3;
extern const float INF = std::numeric_limits<float>::max();




#endif