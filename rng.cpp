/*
 * rng.cpp - part of noisegen
 *
 * Use the new std::random library to generate better and more varied
 * pseudo-random numbers.
 *
 *  This file is part of NoiseGen.
 *  Copyright 2012,15,21 Mark J. Stock and James Sussino
 *
 *  NoiseGen is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  NoiseGen is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NoiseGen.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rng.hpp"

#include <random>
#include <limits>


std::random_device rd;  // Will be used to obtain a seed for the random number engine
std::mt19937 rng(rd()); // Standard mersenne_twister_engine seeded with rd()


//
// Fill a block of floats with uniform random numbers
//
extern "C" int getRandomUniform (
    const RNG userng, const int randSeed,
    float* first, const size_t n,
    const float lower, const float upper) {

  const float scale = upper-lower;

  if (userng == library) {
    srand(randSeed);
    for (size_t i=0; i<n; i++) {
      first[i] = lower + scale*rand()/(float)RAND_MAX;
    }

  } else if (userng == mersenne) {
    std::uniform_real_distribution<float> unit(0.0,1.0);
    rng.seed(randSeed);
    unit.reset();

    for (size_t i=0; i<n; i++) {
      first[i] = lower + scale*unit(rng);
    }
  }

  return n;
}

//
// Fill a block of floats with gaussian random numbers
//
extern "C" int getRandomGaussian (
    const RNG userng, const int randSeed,
    float* first, const size_t n,
    const float mean, const float stddev) {

  if (userng == library) {
    srand(randSeed);
    for (size_t i=0; i<(n+1)/2; i++) {
      float s = 2.0;
      float u = 0.0;
      float v = 0.0;
      while (s == 0.0 || s >= 1.0) {
        u = -1.0 + 2.0*rand()/(float)RAND_MAX;
        v = -1.0 + 2.0*rand()/(float)RAND_MAX;
        s = u*u + v*v;
      }
      const float rad = sqrt(-2.0 * logf(s) / s);
      first[i*2] = mean + stddev * u * rad;
      if (i*2+1 < n) first[i*2+1] = mean + stddev * v * rad;
    }

  } else if (userng == mersenne) {
    std::normal_distribution<float> gaussian{0.0,1.0};
    rng.seed(randSeed);
    gaussian.reset();

    for (size_t i=0; i<n; i++) {
      first[i] = mean + stddev*gaussian(rng);
    }
  }

  return n;
}
