#pragma once
#ifndef GEODESUKA_CORE_MATH_GCLM_BASIC_UTIL_H
#define GEODESUKA_CORE_MATH_GCLM_BASIC_UTIL_H

#include <stdarg.h>

#include "../config.h"

/*
* This is supposed to be a backend library for the other classes.
* levi_cevita is used to find determinants of NxN matrices.
* can be extended to any size matrix to compute determinant.
*/

int kronecker_delta(int I, int J);
int levi_civita(int aRank, const int* aIndex);
int levi_civita(int aRank, ...);

#endif // !GEODESUKA_CORE_MATH_GCLM_BASIC_UTIL_H
