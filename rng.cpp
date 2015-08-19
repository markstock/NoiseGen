/*
 * rng.cpp - part of noisegen
 *
 * Use the Boost:Random library to generate better and more varied
 * pseudo-random numbers.
 *
 * g++ -c rng.cpp -I/mnt/third_party/boost/boost_1_49_0/
 *
 *  This file is part of NoiseGen.
 *  Copyright 2012,5 Mark J. Stock and James Sussino
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

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <limits>

#include <boost/version.hpp>
#if BOOST_VERSION >= 104700
#define UNIF_REAL uniform_real_distribution
#else
#define UNIF_REAL uniform_real
#endif

using namespace boost;
using namespace boost::random;

// this is the high-quality, good-performance RNG that I like
mt19937 rng;


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
    UNIF_REAL<float> unit(0.0,1.0);
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

  } else if (0) {
    // I thought this would work for userng == mersenne, but it doesn't
    normal_distribution<float> gaussian(0.0,1.0);
    rng.seed(randSeed);
    gaussian.reset();

    for (size_t i=0; i<n; i++) {
      first[i] = mean + stddev*gaussian(rng);
    }

  } else if (userng == mersenne) {
    // so use Knop's form of the Box-Mueller transform, as above
    UNIF_REAL<float> unit(-1.0,1.0);
    rng.seed(randSeed);
    unit.reset();

    for (size_t i=0; i<(n+1)/2; i++) {
      float s = 2.0;
      float u = 0.0;
      float v = 0.0;
      while (s == 0.0 || s >= 1.0) {
        u = unit(rng);
        v = unit(rng);
        s = u*u + v*v;
      }
      const float rad = sqrt(-2.0 * logf(s) / s);
      first[i*2] = mean + stddev * u * rad;
      if (i*2+1 < n) first[i*2+1] = mean + stddev * v * rad;
    }
  }

  return n;
}
