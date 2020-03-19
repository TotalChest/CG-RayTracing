#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <cmath>
#include <vector>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <omp.h>
#include "geometry.h"



int threads = 8;
extern const int HEIGHT = 900;
extern const int WIDTH  = 1600;
extern const float PI = 3.1415926535;
extern const float EPSILON = 0.0001;
extern const int RECURSION_DEPTH = 5;
extern const float INF = 10000;
const Color Back_ground(15, 0, 35);



#endif