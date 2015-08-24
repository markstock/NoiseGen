/*
 * fft.h
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

#ifndef FFT_H
#define FFT_H

#include <stdint.h>
#include "planes.h"

// Windows lacks fminf
#ifndef fminf
#define fminf(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef fmaxf
#define fmaxf(x, y) (((x) > (y)) ? (x) : (y))
#endif

int shiftSpectrum1D (float*, const size_t, const float, const float, const float);

void* decompose2D (float*, const size_t, const size_t);
int shiftPowerSpectrum2D (void*, const size_t, const size_t, const float, const float, const float);
int addPlanesToSpectrum2D (void*, const size_t, const size_t, const uint32_t, const PLANE*);
int reproject2D (void*, const size_t, const size_t, float*);

void* decompose3D (float*, const size_t, const size_t, const size_t);
int shiftPowerSpectrum3D (void*, const size_t, const size_t, const size_t, const float, const float, const float);
int addPlanesToSpectrum3D (void*, const size_t, const size_t, const size_t, const uint32_t, const PLANE*);
int reproject3D (void*, const size_t, const size_t, const size_t, float*);

int forward1Dfc (float*, const size_t);
int inverse1Dfc (float*, const size_t);

#endif // FFT_H
