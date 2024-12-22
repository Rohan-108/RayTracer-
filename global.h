#ifndef GLOBAL_H
#define GLOBAL_H

// common headers
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"

using std::make_shared;
using std::shared_ptr;
// constants
const double infinity = std::numeric_limits<double>::infinity();

const double pi = 3.1415926535897932385;

// utility function

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

#endif