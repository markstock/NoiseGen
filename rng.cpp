/*
 * rng.cpp - part of noisegen
 *
 * Use the Boost:Random library to generate better and more varied
 * pseudo-random numbers.
 *
 * g++ -c rng.cpp -I/mnt/third_party/boost/boost_1_49_0/
 *
 *  This file is part of NoiseGen.
 *  Copyright 2012 Mark J. Stock and James Sussino
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


float randFloat (void) {
  UNIF_REAL<float> unit(0.0,1.0);
  return (unit(rng));
}

float randGauss (void) {
  normal_distribution<float> gaussian(0.0,1.0);
  return (gaussian(rng));
}

//
// Fill a block of floats with uniform random numbers
//
extern "C" int getRandomUniform (const RNG userng,
    float* first, const size_t n,
    const float lower, const float upper) {

  const float scale = upper-lower;

  if (userng == library) {
    for (size_t i=0; i<n; i++) {
      first[i] = lower + scale*rand()/(float)RAND_MAX;
    }

  } else if (userng == mersenne) {
    for (size_t i=0; i<n; i++) {
      first[i] = lower + scale*randFloat();
    }
  }

  return n;
}

//
// Fill a block of floats with gaussian random numbers
//
extern "C" int getRandomGaussian (const RNG userng,
    float* first, const size_t n,
    const float mean, const float stddev) {

  if (userng == library) {
    // NOTE: does not use library rand() routine!
    for (size_t i=0; i<n; i++) {
      first[i] = mean + stddev*randGauss();
    }

  } else if (userng == mersenne) {
    for (size_t i=0; i<n; i++) {
      first[i] = mean + stddev*randGauss();
    }
  }

  return n;
}
