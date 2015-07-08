/*
 * output3d.c
 *
 * 3-D signal output, either text, raw, bob, or bos
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

#include "output3d.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>


void writeData3D (OUTFF type, char* outfile, float *outdata,
    size_t nx, size_t ny, size_t nz) {

  // output handle defaults to stdout
  FILE* ofh = stdout;

  // write the data to the output file handle using the proper file type
  if (type == raw) {

    if (outfile) ofh = fopen(outfile,"wb");
    fwrite(outdata,sizeof(float),nx*ny*nz,ofh);
    if (outfile) fclose(ofh);

  } else if (type == text) {

    if (outfile) ofh = fopen(outfile,"w");
    for (size_t i=0; i<nx*ny*nz; i++)
      fprintf(ofh,"%d %d %d %g\n",(int)(i/(ny*nz)),(int)((i/nz)%ny),(int)(i%nz),outdata[i]);
    if (outfile) fclose(ofh);

  } else if (type == bob) {

    // find the min/max
    float datmin = FLT_MAX;
    float datmax = FLT_MIN;
    for (size_t i=0; i<nx*ny*nz; i++) {
      if (outdata[i] < datmin) datmin = outdata[i];
      if (outdata[i] > datmax) datmax = outdata[i];
    }
    // scale the data to the range of a byte
    char* brick = (char*) malloc(sizeof(char)*nx*ny*nz);
    for (size_t i=0; i<nx*ny*nz; i++)
      brick[i] = (char)(255.999 * (outdata[i]-datmin) / (datmax-datmin));

    if (outfile) ofh = fopen(outfile,"wb");
    uint32_t outputRes = nx;
    fwrite(&outputRes,sizeof(uint32_t),1,ofh);
    outputRes = ny;
    fwrite(&outputRes,sizeof(uint32_t),1,ofh);
    outputRes = nz;
    fwrite(&outputRes,sizeof(uint32_t),1,ofh);
    // then write the data
    fwrite(brick,sizeof(char),nx*ny*nz,ofh);
    if (outfile) fclose(ofh);

    free(brick);

  } else if (type == bos) {

    // find the min/max
    float datmin = FLT_MAX;
    float datmax = FLT_MIN;
    for (size_t i=0; i<nx*ny*nz; i++) {
      if (outdata[i] < datmin) datmin = outdata[i];
      if (outdata[i] > datmax) datmax = outdata[i];
    }
    // scale the data to the range of a byte
    uint16_t* brick = (uint16_t*) malloc(sizeof(uint16_t)*nx*ny*nz);
    for (size_t i=0; i<nx*ny*nz; i++)
      brick[i] = (uint16_t)(65535.9 * (outdata[i]-datmin) / (datmax-datmin));

    if (outfile) ofh = fopen(outfile,"wb");
    uint32_t outputRes = nx;
    fwrite(&outputRes,sizeof(uint32_t),1,ofh);
    outputRes = ny;
    fwrite(&outputRes,sizeof(uint32_t),1,ofh);
    outputRes = nz;
    fwrite(&outputRes,sizeof(uint32_t),1,ofh);
    // then write the data
    fwrite(brick,sizeof(uint16_t),nx*ny*nz,ofh);
    if (outfile) fclose(ofh);

    free(brick);

  } else {
    fprintf(stderr,"ERROR (writeData3D): output file type unsupported.\n");
  }

  return;
}

