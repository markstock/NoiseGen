/*
 * planes.h
 *
 * define arbitrary-dimensional planes which to accentuate
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

#pragma once

#include "noisegen.h"

#define MAXPLANES 100

//
// When given a positive strength, will cause directional
// streaks in 2D data and co-planar planes in 3D data;
// with strength=-1, will cut out those streaks or planes.
// More than one plane can be added, the effects are
// accumulated, not multiplied.
//
typedef struct planeDefinitionType {
  float vec[MAXDIMS];
  float width;
  float strength;
} PLANE;

