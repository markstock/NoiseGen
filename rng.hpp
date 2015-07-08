/*
 * rng.h
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

#ifndef RNG_H
#define RNG_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stdint.h>
#endif

typedef enum randomNumberGeneratorType {library,mersenne} RNG;

#ifdef __cplusplus
extern "C"
#endif
int getRandomUniform (const RNG, float*, const size_t, const float, const float);

#ifdef __cplusplus
extern "C"
#endif
int getRandomGaussian (const RNG, float*, const size_t, const float, const float);

#endif // RNG_H
