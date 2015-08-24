/*
 * fft3d.c - part of noisegen
 *
 * Use FFTW to perform forward and inverse Fast Fourier Transforms.
 *
 * link with -lfftw3f
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
#include <math.h>
#include <fftw3.h>
#include "fft.h"
#include "output2d.h"

#define M_PI 3.14159265358979323846

#define DEBUG FALSE

//void writeSpectrum2D (OUTFF, char*, fftwf_complex*, size_t, size_t);


/*
 * Take any real 3D signal, r2c forward transform,
 */
void* decompose3D (float* in,
    const size_t nx, const size_t ny, const size_t nz) {

  fftwf_complex* data;
  fftwf_plan pforward;

  // the working data, complex
  data = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * nx * ny * (nz/2+1));

  // the forward
  pforward = fftwf_plan_dft_r2c_3d(nx, ny, nz, in, data, FFTW_ESTIMATE);

  // execute the forward DFT
  fftwf_execute(pforward);

  // clear the temporary object
  fftwf_destroy_plan(pforward);

  // DEBUG print the input
  if (DEBUG) {
    writeData2D(png,"tempoutA.png",in,ny,nz);
  }

  // DEBUG print the frequency components
  if (DEBUG) {
    //writeSpectrum2D(png,"tempoutB.png",data,ny,nz);
  }

  return data;
}


/*
 * Take the complex 3D spectrum and shift the power relationship
 */
int shiftPowerSpectrum3D (void *inout,
    const size_t nx, const size_t ny, const size_t nz,
    const float longestWavelength, const float shortestWavelength,
    const float exponent) {

  fftwf_complex* data = (fftwf_complex*)inout;

  // in 3D, we need to adjust the exponent
  // it's /2 because we need to take the sqrt of the distance from the origin!
  const float thisexp = exponent / 2.0;

  // scale the frequency components
  for (size_t i=0; i<nx; i++) {
    for (size_t j=0; j<ny; j++) {
      for (size_t k=0; k<nz/2+1; k++) {

        float diag = 1 + k*k;
        if (j < ny/2) diag += j*j;
        else diag += (ny-j)*(ny-j);
        if (i < nx/2) diag += i*i;
        else diag += (nx-i)*(nx-i);

        float factor = pow(diag, thisexp);
        size_t ijk = (i*ny + j)*(nz/2+1) + k;
        data[ijk][0] *= factor;
        data[ijk][1] *= factor;

        // band-pass filter by wavelength
        if (ijk != 0) {
          const float wavelength = 1.0 / sqrt(diag - 1.0);
          if (wavelength > longestWavelength && longestWavelength > 0.0) {
            data[ijk][0] = 0.0;
            data[ijk][1] = 0.0;
          }
          if (wavelength < shortestWavelength && shortestWavelength > 0.0) {
            data[ijk][0] = 0.0;
            data[ijk][1] = 0.0;
          }
        }
      }
    }
  }
  float dcSignal = data[0][0] / ((float)ny*(float)nx);
  fprintf(stderr,"dc signal is %g\n",dcSignal);

  // DEBUG print the frequency components
  if (DEBUG) {
    float* temp = (float*)malloc(sizeof(float)*nx*(ny/2+1));
    for (size_t i=0; i<nx*(ny/2+1); i++) temp[i] = data[i][0];
    writeData2D(png,"tempoutC.png",temp,nx,ny/2+1);
    free(temp);
  }
  if (DEBUG) {
    //writeSpectrum2D(png,"tempoutC1.png",data,nx,ny);
  }

  return(0);
}


/*
 * Take any complex 3D spectrum and add streaks to the data
 */
int addPlanesToSpectrum3D (void* in,
    const size_t nx, const size_t ny, const size_t nz,
    const uint32_t numPlanes, const PLANE* pp) {

  // re-cast the void* to complex*
  fftwf_complex* data = (fftwf_complex*)in;

  // save the aspect ratio
  // NOT DONE
  const float ar = (float)nx/(float)ny;

  // normalize the plane normal vectors
  // NOT DONE

  // add a streak to the frequency components
  // NOT DONE
  for (size_t i=0; i<nx; i++) {
    for (size_t j=0; j<ny; j++) {
      for (size_t k=0; k<nz/2+1; k++) {
        if (i!=0 && j!=0 && k!=0) {

          float thisAngle;
          if (i < nx/2) thisAngle = atanf(-(float)i/(float)j);
          else thisAngle = atanf((float)(nx-i)/(float)j);

          float factor = 0.0;
          for (uint32_t ip=0; ip < numPlanes; ip++) {

            //float planeAngle = atanf(ar*pp[ip].vec[1]/pp[ip].vec[0]);
            float planeAngle = atanf(ar*pp[ip].vec[0]/pp[ip].vec[1]);
            float diffAngle = fabs(thisAngle-planeAngle);
            // need to make sure we don't miss it in the reflection
            diffAngle = fminf(diffAngle, fabs(thisAngle-planeAngle+M_PI));
            diffAngle = fminf(diffAngle, fabs(thisAngle-planeAngle-M_PI));
            // add up the factors
            factor += pp[ip].strength * fmaxf(0.0, 1.0-diffAngle/pp[ip].width);
          }
          factor += 1.0;

          size_t ijk = (i*ny + j)*(nz/2+1) + k;
          data[ijk][0] *= factor;
          data[ijk][1] *= factor;
        }
      }
    }
  }

  // DEBUG print the frequency components
  if (DEBUG) {
    float* temp = (float*)malloc(sizeof(float)*nx*(ny/2+1));
    for (size_t i=0; i<nx*(ny/2+1); i++) temp[i] = data[i][0];
    writeData2D(png,"tempoutD.png",temp,nx,ny/2+1);
    free(temp);
  }
  if (DEBUG) {
    //writeSpectrum2D(png,"tempoutD1.png",data,nx,ny);
  }

  return(0);
}


/*
 * Take any complex 3D spectrum and c2r inverse transform
 * it back into a real signal
 */
int reproject3D (void* in,
    const size_t nx, const size_t ny, const size_t nz,
    float* out) {

  fftwf_complex* data = (fftwf_complex*)in;

  // the backward plan
  fftwf_plan pinverse = fftwf_plan_dft_c2r_3d(nx, ny, nz, data, out, FFTW_ESTIMATE);

  // then perform an IFT to reconstitute the real signal
  fftwf_execute(pinverse);

  // free the complex data
  fftwf_destroy_plan(pinverse);
  fftwf_free(data);

  // should we normalize?
  float factor = 1. / ((float)ny*(float)nx*(float)nz);
  for (size_t i=0; i<nx*ny*nz; i++) {
    out[i] *= factor;
  }

  return(0);
}


/*
//
// write a more natural-looking image of the spectrum
//
void writeSpectrum2D (OUTFF type, char* outfile,
                      fftwf_complex* in, size_t nx, size_t ny) {

  float* temp = malloc(sizeof(float)*nx*(ny+1));
  for (size_t i=0; i<nx*(ny+1); i++) temp[i] = 0.0;

  // fill the new array with two copies of the real component of the FFT
  for (size_t i=0; i<nx; i++) {
    size_t isrc = (i+nx/2)%nx;
    // bottom first
    for (size_t j=ny/2; j<ny+1; j++) {
      size_t ti = isrc*(ny/2+1) + j-ny/2;
      //temp[i*(ny+1)+j] = in[ti][0];
      temp[i*(ny+1)+j] = sqrt(pow(in[ti][0],2)+pow(in[ti][1],2));
    }
    // then top
    for (size_t j=0; j<ny/2; j++) {
      size_t ti = isrc*(ny/2+1) + ny/2 - j;
      //temp[(nx-i-1)*(ny+1)+j] = in[ti][1];
      temp[(nx-i-1)*(ny+1)+j] = sqrt(pow(in[ti][0],2)+pow(in[ti][1],2));
    }
  }
  writeData2D(type,outfile,temp,nx,ny+1);
  free(temp);
}
*/
