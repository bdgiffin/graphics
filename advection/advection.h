#ifndef ADVECTION_H
#define ADVECTION_H

// include project headers
#include "grid.h"  // Grid
#include "field.h" // Field

// include standard C/C++ libraries
#include <iostream> // exit
#include <cmath>    // abs

class Advection {
  void advect(Grid* grid, Field* velocity);
};

#endif // ADVECTION_H
