/*
 * fft1d.c
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


/*
 * Take any real signal, r2c forward transform,
 * shift the spectrum, trying to maintain equivalent peak-peak,
 * then c2r inverse transform it back into a real signal
 */
int shiftSpectrum1D (float *inout, const size_t n,
    const float longestWavelength, const float shortestWavelength,
    const float exponent) {

  fftwf_complex *data;
  fftwf_plan pforward, pinverse;

  // the working data, complex
  data = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (n/2+1));

  // the forward and backward plans
  pforward = fftwf_plan_dft_r2c_1d(n, inout, data, FFTW_ESTIMATE);
  pinverse = fftwf_plan_dft_c2r_1d(n, data, inout, FFTW_ESTIMATE);

  // execute the forward DFT
  fftwf_execute(pforward);

  // scale the frequency components
  for (size_t i=1; i<n/2+1; i++) {
    const float factor = pow((float)(1+n/2-i), exponent);
    //printf("%d  %g %g   %g %g\n",i,data[i][0],data[i][1],(float)(1+n/2-i),factor);
    //printf("%d  %g\n",i,sqrt(pow(data[i][0],2)+pow(data[i][1],2)));
    data[i][0] *= factor;
    data[i][1] *= factor;
  }

  // band-pass filter the frequency components
  if (longestWavelength > 0.0) {
    for (size_t i=1; i<n/2+1; i++) {
      const float wavelength = 1.0 / (float)(1+n/2-i);
      if (wavelength > longestWavelength) {
        data[i][0] = 0.0;
        data[i][1] = 0.0;
      }
    }
  }
  if (shortestWavelength > 0.0) {
    for (size_t i=1; i<n/2+1; i++) {
      const float wavelength = 1.0 / (float)(1+n/2-i);
      if (wavelength < shortestWavelength) {
        data[i][0] = 0.0;
        data[i][1] = 0.0;
      }
    }
  }

  float dcSignal = data[0][0]/(float)n;
  fprintf(stderr,"dc signal is %g\n",dcSignal);

  // then perform an IFT to reconstitute the real signal
  fftwf_execute(pinverse);

  // should we normalize?
  float factor = 1./(float)n;
  for (size_t i=0; i<n; i++) {
    inout[i] *= factor;
  }

  // adjust odd values to other side of DC signal
  for (size_t i=1; i<n; i+=2) {
    inout[i] = 2*dcSignal - inout[i];
  }

  return(0);
}


/*
 * Forward FFT
 * Converts a signal into its frequency components
 */
int forward1Dfc (float *inout, const size_t n) {

  fftwf_complex *data;
  fftwf_plan p;

  data = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * n);
  p = fftwf_plan_dft_1d(n, data, data, FFTW_FORWARD, FFTW_ESTIMATE);

  // fill the arrays
  for (size_t i=0; i<n; i++) {
    data[i][0] = inout[i];
    data[i][1] = 0.0;
  }

  fftwf_execute(p); /* repeat as needed */

  // put data back into original array
  for (size_t i=0; i<n; i++) {
    inout[i] = data[i][0];
    //printf("%d  %g  %g\n",i,data[i][0],data[i][1]);
  }

  fftwf_destroy_plan(p);
  fftwf_free(data);

  return(0);
}

/*
 * Inverse FFT
 * Converts frequency components back into a signal
 */
int inverse1Dfc (float *inout, const size_t n) {

  fftwf_complex *data;
  fftwf_plan p;

  data = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * n);
  p = fftwf_plan_dft_1d(n, data, data, FFTW_BACKWARD, FFTW_ESTIMATE);

  // fill the arrays
  for (size_t i=0; i<n; i++) {
    data[i][0] = inout[i];
    data[i][1] = 0.0;
  }

  fftwf_execute(p); /* repeat as needed */

  // put data back into original array
  for (size_t i=0; i<n; i++) {
    inout[i] = data[i][0];
  }

  fftwf_destroy_plan(p);
  fftwf_free(data);

  return(0);
}


