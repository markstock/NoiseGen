/*
 * output1d.h
 *
 * 1-D signal output, either text or WAV
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

#ifndef OUTPUT1D_H
#define OUTPUT1D_H

#include <stdio.h>
#include <stdint.h>
#include "output.h"

void writeData1D (OUTFF, char*, float*, size_t);
void writeSpectrum1D (OUTFF, char*, float*, size_t);

#endif // OUTPUT1D_H
