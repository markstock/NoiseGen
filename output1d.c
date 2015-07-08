/*
 * output1d.c
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

#include <stdio.h>
#include "output1d.h"

void writeData1D (OUTFF type, char* outfile, float *outdata, size_t n) {

  // output handle defaults to stdout
  FILE* ofh = stdout;

  // write the resulting signal to the output file handle using the proper file type
  if (type == raw) {
    if (outfile) ofh = fopen(outfile,"wb");
    fwrite(outdata,sizeof(float),n,ofh);
    if (outfile) fclose(ofh);

  } else if (type == text) {
    if (outfile) ofh = fopen(outfile,"w");
    for (size_t i=0; i<n; i++)
      fprintf(ofh,"%d %g\n",(int)i,outdata[i]);
    if (outfile) fclose(ofh);

  } else if (type == wav) {
    fprintf(stderr,"ERROR (writeData1D): output file type .wav unsupported.\n");

  } else {
    fprintf(stderr,"ERROR (writeData1D): output file type unsupported.\n");

  }

  return;
}

void writeSpectrum1D (OUTFF type, char* outfile, float *outdata, size_t n) {

  // perform the FFT and dump the frequency components


  return;
}
