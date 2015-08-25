/*
 * noisegen
 *
 * (c)2012,5 Mark J. Stock  mstock@umich.edu
 *       and James Susinno  james.susinno@gmail.com
 *
 * Program to generate and write noise data
 *
 * Background info at http://en.wikipedia.org/wiki/Colors_of_noise
 *
 * All noise is computed using 32-bit floats, but output will often be
 * quantized into 8, 16, or 32-bit ints (16-bit for png)
 *
 * 0.1	2012-07-29  MJS  First packaging, 1D with color
 * 0.2  2012-08-19  MJS  Added boost::random Mersenne twister rng
 * 0.3  2012-08-19  MJS  2D generation, coloring, and PNG output
 * 0.4  2012-08-20  MJS  3D generation, coloring, and BOB output
 * 0.5  2012-08-21  MJS  2D support for preference planes, non-square output
 * 0.6  2012-08-26  MJS  3D support for preference planes, brick-of-shorts
 * 0.7  2015-08-19  MJS  Fixed Gaussian numbers, added seed
 *
 * Build with:
 * % cmake .
 * % make
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "noisegen.h"
#include "fft.h"
#include "rng.hpp"
#include "output.h"
#include "output1d.h"
#include "output2d.h"
#include "output3d.h"
#include "planes.h"

void blur2D(float*, size_t, size_t);
int Usage(char[255], int);

typedef enum noiseColorType {white,pink,red,brown,blue,violet} COLOR;
typedef enum noisePdfType {uniform,Gaussian} PDF;


int main (int argc, char **argv) {

  //-------------------------------------------------------------------------
  // declare variables and set defaults

  // how many dimensions of noise do we want?
  uint8_t numDims = 1;
  // data array size for each dimension
  uint32_t n[MAXDIMS] = {100,1,1};
  // arrays for the data itself
  float* data;
  // DC voltage (mean signal)
  float dc = 0.0;
  // noise color (spectrum, related to autocorrelation)
  COLOR noiseColor = white;
  float powerExp = 1.0;
  // user-input exponent
  BOOL useInputExponent = FALSE;
  float inputExponent = 0.0;
  // bandpass filter bounds
  float longestWavelength = -1.0;
  float shortestWavelength = -1.0;
  // preference planes/vectors
  uint32_t numPlanes = 0;
  PLANE planes[MAXPLANES];
  // zero mean?
  BOOL zeroMean = FALSE;
  // noise probabilty distribution function
  PDF noisePdf = uniform;
  // random number generator
  RNG generator = mersenne;
  // seed for rng
  int randSeedVal = 23516;
  // output file types
  OUTFF outtype = text;
  char* outfile = NULL;
  

  //-------------------------------------------------------------------------
  // read command line

  char progname[255];
  (void) strcpy(progname,argv[0]);
  if (argc < 2) (void) Usage(progname,0);
  for (uint32_t i=1; i<argc; i++) {

    if (strncmp(argv[i], "-d", 2) == 0) {
      numDims = atoi(argv[++i]);

    } else if (strncmp(argv[i], "-n", 2) == 0) {
      n[0] = (size_t)atol(argv[++i]);
      if (argc > i+1) {
        if (isdigit((int)argv[i+1][0])) {
          n[1] = (size_t)atol(argv[++i]);
          if (argc > i+1) {
            if (isdigit(argv[i+1][0])) {
              n[2] = (size_t)atol(argv[++i]);
            }
          }
        }
      }

    } else if (strncmp(argv[i], "-o", 2) == 0) {
      outfile = (char*) malloc(255*sizeof(char));
      strcpy(outfile,argv[++i]);
    } else if (strncmp(argv[i], "-lib", 4) == 0) {
      generator = library;
    } else if (strncmp(argv[i], "-zero", 2) == 0) {
      zeroMean = TRUE;
    } else if (strncmp(argv[i], "-seed", 5) == 0) {
      randSeedVal = (int)atoi(argv[++i]);

    } else if (strncmp(argv[i], "-short", 3) == 0) {
      shortestWavelength = (float)atof(argv[++i]);
    } else if (strncmp(argv[i], "-long", 3) == 0) {
      longestWavelength = (float)atof(argv[++i]);


    } else if (strncmp(argv[i], "-red", 4) == 0) {
      noiseColor = red;
    } else if (strncmp(argv[i], "-brown", 5) == 0) {
      noiseColor = brown;
    } else if (strncmp(argv[i], "-pink", 5) == 0) {
      noiseColor = pink;
    } else if (strncmp(argv[i], "-white", 5) == 0) {
      noiseColor = white;
    } else if (strncmp(argv[i], "-blue", 5) == 0) {
      noiseColor = blue;
    } else if (strncmp(argv[i], "-violet", 5) == 0) {
      noiseColor = violet;
    } else if (strncmp(argv[i], "-e", 2) == 0) {
      inputExponent = (float)atof(argv[++i]);
      useInputExponent = TRUE;

    } else if (strncmp(argv[i], "-u", 2) == 0) {
      noisePdf = uniform;

    } else if (strncmp(argv[i], "-g", 2) == 0) {
      noisePdf = Gaussian;

    } else if (strncmp(argv[i], "-p", 2) == 0) {
      planes[numPlanes].vec[0] = (float)atof(argv[++i]);
      planes[numPlanes].vec[1] = (float)atof(argv[++i]);
      planes[numPlanes].vec[2] = (float)atof(argv[++i]);
      planes[numPlanes].width = fabs(atof(argv[++i]));
      planes[numPlanes].strength = (float)atof(argv[++i]);
      numPlanes++;

    } else {
      fprintf(stderr,"Unknown option (%s)\n",argv[i]);
      (void) Usage(progname,0);
    }
  }

  //-------------------------------------------------------------------------
  // vet inputs
  if (numDims < 1 || numDims > SUPPORTEDDIMS) {
    fprintf(stderr,"ERROR: number of dimensions must be 1..%d\n",SUPPORTEDDIMS);
    exit(1);
  }

  for (uint8_t i=0; i<MAXDIMS; i++) {
    if (n[i] > UINT32_MAX/2) {
      fprintf(stderr,"ERROR: You're asking for over 2.1 billion points...(%d)\n",n[i]);
      exit(1);
    }
  }
  float totalN = 1.0;
  for (uint8_t i=0; i<MAXDIMS; i++) totalN *= (float)n[i];
  if (totalN > (float)(UINT32_MAX/2)) {
    fprintf(stderr,"ERROR: You're asking for over 2.1 billion total points...(%g)\n",totalN);
    exit(1);
  }

  // parse the input file name for file type
  // was a file name given?
  if (outfile) {
    // is there an extension?
    char* dotptr = strrchr(outfile,'.');
    if (dotptr) {
      // advance the pointer to the next character
      dotptr++;
      // does the extension match any of the predefined output types?
      if (strncmp(dotptr, "t", 1) == 0) {
        outtype = text;
      } else if (strncmp(dotptr, "raw", 3) == 0) {
        outtype = raw;
      } else if (strncmp(dotptr, "png", 3) == 0) {
        outtype = png;
      } else if (strncmp(dotptr, "wav", 3) == 0) {
        outtype = wav;
      } else if (strncmp(dotptr, "bob", 3) == 0) {
        outtype = bob;
      } else if (strncmp(dotptr, "bos", 3) == 0) {
        outtype = bos;
      }
    }
  }


  // convert the color to a power-law exponent
  if (noiseColor == red || noiseColor == brown) {
    // Brownian (1/f^2) noise
    powerExp = -2.0;

  } else if (noiseColor == pink) {
    // pink (1/f) noise
    powerExp = -1.0;

  } else if (noiseColor == violet) {
    // violet (f^2) noise
    powerExp = 2.0;

  } else if (noiseColor == blue) {
    // blue (f) noise
    powerExp = 1.0;
  }

  // but if a power exponent was explicitly given, use that instead
  if (useInputExponent) {
    powerExp = inputExponent;
  }


  //-------------------------------------------------------------------------
  // split on number of dimensions
  if (numDims == 1) {

    // make space for the data
    data = (float*) malloc(n[0]*sizeof(float));

    // generate white (uncorrelated) noise
    // split on sample distribution
    if (noisePdf == uniform) {
      getRandomUniform(generator,randSeedVal,data,n[0],-1.0,1.0);
    } else if (noisePdf == Gaussian) {
      getRandomGaussian(generator,randSeedVal,data,n[0],0.0,1.0);
    }

    // shift power spectrum
    if (noiseColor != white || useInputExponent)
      shiftSpectrum1D(data,n[0],longestWavelength,shortestWavelength,powerExp);

    // write resulting data
    (void) writeData1D (outtype, outfile, data, n[0]);
    //(void) writeSpectrum1D (outtype, outfile, data, n[0]);


  //-------------------------------------------------------------------------
  } else if (numDims == 2) {

    data = (float*) malloc(n[0]*n[1]*sizeof(float*));

    // generate white (uncorrelated) noise
    // split on sample distribution
    if (noisePdf == uniform) {
      getRandomUniform(generator,randSeedVal,data,n[0]*n[1],-1.0,1.0);
    } else if (noisePdf == Gaussian) {
      getRandomGaussian(generator,randSeedVal,data,n[0]*n[1],0.0,1.0);
    }

    // play around a little
    if (FALSE) {
      writeData2D (outtype, "tempout0.png", data, n[0], n[1]);
      blur2D(data,n[0],n[1]);
    }

    // shift power spectrum
    if (noiseColor != white || useInputExponent || numPlanes > 0) {

      // generate the complex frequency spectrum
      void* interim = decompose2D(data,n[0],n[1]);

      // shift it to color the noise
      shiftPowerSpectrum2D(interim,n[0],n[1],longestWavelength,shortestWavelength,powerExp);

      // add spikes emanating from the origin in f space
      addPlanesToSpectrum2D(interim,n[0],n[1],numPlanes,planes);

      // reconstitute the signal
      reproject2D(interim,n[0],n[1],data);
    }

    // renormalize
    if (zeroMean)
      normalizeInPlace(data,n[0]*n[1]);

    // write resulting data
    writeData2D (outtype, outfile, data, n[0], n[1]);


  //-------------------------------------------------------------------------
  } else if (numDims == 3) {

    data = (float*) malloc(n[0]*n[1]*n[2]*sizeof(float*));

    // generate white (uncorrelated) noise
    // split on sample distribution
    if (noisePdf == uniform) {
      getRandomUniform(generator,randSeedVal,data,n[0]*n[1]*n[2],-1.0,1.0);
    } else if (noisePdf == Gaussian) {
      getRandomGaussian(generator,randSeedVal,data,n[0]*n[1]*n[2],0.0,1.0);
    }

    // shift power spectrum
    if (noiseColor != white || useInputExponent || numPlanes > 0) {

      // generate the complex frequency spectrum
      void* interim = decompose3D(data,n[0],n[1],n[2]);

      // shift it to color the noise
      shiftPowerSpectrum3D(interim,n[0],n[1],n[2],longestWavelength,shortestWavelength,powerExp);

      // add spikes emanating from the origin in f space
      // NOT DONE
      //addPlanesToSpectrum3D(interim,n[0],n[1],n[2],numPlanes,planes);

      // reconstitute the signal
      reproject3D(interim,n[0],n[1],n[2],data);
    }

    // write resulting data
    writeData3D (outtype, outfile, data, n[0], n[1], n[2]);
  }


  //-------------------------------------------------------------------------
  // echo a summary of the contents of the output data
  if (totalN > (float)(UINT32_MAX)) 
    fprintf(stderr,"Created %g data points\n",totalN);
  else
    fprintf(stderr,"Created %d data points\n",(uint32_t)totalN);

  // all's well?
  exit(0);
}


//
// Do a simple blur in physical space
//
void blur2D(float *data, size_t nx, size_t ny) {

  // a temporary array
  float* temp = (float*)malloc(sizeof(float)*nx*ny);

  // how many iterations?
  for (size_t iter=0; iter<10; iter++) {

    // loop over internal pixels only
    for (size_t i=0; i<nx*ny; i++) temp[i] = data[i];
    for (size_t i=1; i<nx-1; i++) {
      for (size_t j=1; j<ny-1; j++) {
        size_t idx = i*ny + j;
        data[idx] += (temp[idx+1]+temp[idx-1]+temp[idx+ny]+temp[idx-ny]-4.*temp[idx]) / 16.0;
      }
    }
  }
  free(temp);
  return;
}


/*
 * This function writes basic usage information to stderr,
 * and then quits. Too bad.
 */
int Usage(char progname[255], int status) {

  static char **cpp, *help_message[] = {
  "where [-options] are one or more of the following:                         ",
  "                                                                           ",
  "   -d [int]    number of dimensions, 1-3; default=1                        ",
  "                                                                           ",
  "   -n [int [int [int]]]  number of entries in each dimension; default=10   ",
  "                                                                           ",
  "   -red        generate red (1/f^2) noise                                  ",
  "                                                                           ",
  "   -brown      generate brown (1/f^2) noise                                ",
  "                                                                           ",
  "   -pink       generate pink (1/f) noise                                   ",
  "                                                                           ",
  "   -white      generate white (f^0) noise [default behavior]               ",
  "                                                                           ",
  "   -blue       generate blue (f) noise                                     ",
  "                                                                           ",
  "   -violet     generate violet (f^2) noise                                 ",
  "                                                                           ",
  "   -e [real]   specify power law for noise                                 ",
  "                                                                           ",
  "   -long [real]  clip any wavelength longer than that given                ",
  "                                                                           ",
  "   -short [real]  clip any wavelength shorted than that given              ",
  "                                                                           ",
  "   -u          use uniform random numbers [default behavior]               ",
  "                                                                           ",
  "   -g          use Gaussian random numbers                                 ",
  "                                                                           ",
  "   -seed [int]  use the given random seed, otherwise seed is fixed         ",
  "                                                                           ",
  "   -lib        use C standard library random number generator instead      ",
  "               of mt19937 from boost::random                               ",
  "                                                                           ",
  "   -p x y z width strength                                                 ",
  "               adds a preference plane to the output, x,y,z is the         ",
  "               plane normal, width specifies the arc width in the          ",
  "               frequency domain (0.05-1.0 is a useful range) and           ",
  "               strength scales the effect, where 1.0 doubles the           ",
  "               prevalence of the plane above random, -1.0 quiets the       ",
  "               plane, and 10.0 highly accentuates it; this option          ",
  "               is useful only in 2D and 3D output, and can be entered      ",
  "               multiple times                                              ",
  "                                                                           ",
  "   -o name     specify output file name AND format;                        ",
  "               supported formats: txt raw png bob bos                      ",
  " ",
  "Options may be abbreviated to an unambiguous length.",
  " ",
  "Examples",
  " ",
  "   noisegen -n 10000 -e -1.5",
  "      Writes noise with a power spectrum of f^(-1.5) as text to stdout",
  " ",
  "   noisegen -d 2 -n 512 512 -brown -o brown.png",
  "      Creates a 2D PNG image of fractional Brownian noise",
  " ",
  "   noisegen -d 2 -n 100 100 -blue -u -o blue.raw",
  "      Creates a raw binary file of 10000 4-byte floats of blue noise",
  " ",
  "   noisegen -d 3 -n 64 64 64 -g -o brick.bob",
  "      Creates a 3D 'brick of bytes' file of Gaussian white noise",
  " ",
  " ",
  NULL
  };

  fprintf(stderr, "usage:\n  %s [options]\n\n", progname);
  for (cpp = help_message; *cpp; cpp++) fprintf(stderr, "%s\n", *cpp);
  fflush(stderr);
  exit(status);
  return(0);
}
