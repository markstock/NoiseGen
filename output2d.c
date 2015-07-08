/*
 * output2d.c
 *
 * 2-D signal output, either text or WAV
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

#include "output2d.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include "png.h"

int writePng (char*, float*, size_t, size_t);

png_byte** allocate_2d_array_pb (size_t,size_t,int);
int free_2d_array_pb (png_byte**);

void writeData2D (OUTFF type, char* outfile, float *outdata,
    size_t nx, size_t ny) {

  // output handle defaults to stdout
  FILE* ofh = stdout;

  // write the data to the output file handle using the proper file type
  if (type == raw) {

    if (outfile) ofh = fopen(outfile,"wb");
    fwrite(outdata,sizeof(float),nx*ny,ofh);
    if (outfile) fclose(ofh);

  } else if (type == text) {

    if (outfile) ofh = fopen(outfile,"w");
    for (size_t i=0; i<nx*ny; i++)
      fprintf(ofh,"%d %d %g\n",(int)(i/ny),(int)(i%ny),outdata[i]);
    if (outfile) fclose(ofh);

  } else if (type == png) {

    (void) writePng(outfile,outdata,nx,ny);

  } else {
    fprintf(stderr,"ERROR (writeData2D): output file type unsupported.\n");
  }

  return;
}


//
// Scale the input, call libpng, and write a 16-bit image file
//
// PNGs are stored in column-major format, so only here do we 
// shuffle the order around.
//
int writePng (char *outfilename, float *outdata, size_t nx, size_t ny) {

  int autorange = 1; // true
  int i,j,printval;
  int bit_depth = 16;
  float newminrange,newmaxrange;
  float valmin,valrange;
  // gamma of 1.8 looks normal on most monitors...that display properly.
  //float gamma = 1.8;
  // must do 5/9 for stuff to look right on Macs....why? I dunno.
  float gamma = .55555;
  png_uint_32 width = nx;
  png_uint_32 height = ny;
  png_structp png_ptr;
  png_infop info_ptr;
  png_byte **img;
  FILE *fp = stdout;

  fprintf(stderr,"Writing %s\n",outfilename);

  // allocate the space for the byte array
  // this is one place where we switch x and y
  img = allocate_2d_array_pb(ny,nx,bit_depth);

  // compute the range
  newminrange = FLT_MAX;
  newmaxrange = FLT_MIN;
  float* valptr = outdata;
  for (i=0; i<nx*ny; i++) {
    if (*valptr < newminrange) newminrange = *valptr;
    if (*valptr > newmaxrange) newmaxrange = *valptr;
    valptr++;
  }

  // auto-set the ranges
  if (autorange) {
    fprintf(stderr,"  range %g %g\n",newminrange,newmaxrange);
    valmin = newminrange;
    valrange = newmaxrange-newminrange;
  } else {
    fprintf(stderr,"  output range %g %g\n",newminrange,newmaxrange);
  }

  // write the file
  if (outfilename) {
    fp = fopen(outfilename,"wb");
    if (fp==NULL) {
      fprintf(stderr,"Could not open output file %s\n",outfilename);
      fflush(stderr);
      exit(0);
    }
  }

  // here is the other place where we switch x and y
  // no scaling, 16-bit per channel
  if (bit_depth == 16) {
    float* valptr = outdata;
    for (i=0; i<nx; i++) {
      for (j=0; j<ny; j++) {
        printval = (int)(65535.9*(*valptr-valmin)/valrange);
        if (printval<0) printval = 0;
        else if (printval>65535) printval = 65535;
        img[j][2*i] = (png_byte)(printval/256);
        img[j][2*i+1] = (png_byte)(printval%256);
        valptr++;
      }
    }

  // no scaling, 8-bit per channel
  } else {
    float* valptr = outdata;
    for (i=0; i<nx; i++) {
      for (j=0; j<ny; j++) {
        printval = (int)(255.999*(*valptr-valmin)/valrange);
        if (printval<0) printval = 0;
        else if (printval>255) printval = 255;
        img[j][i] = (png_byte)printval;
        valptr++;
      }
    }
  }

  /* Create and initialize the png_struct with the desired error handler
   * functions.  If you want to use the default stderr and longjump method,
   * you can supply NULL for the last three parameters.  We also check that
   * the library version is compatible with the one used at compile time,
   * in case we are using dynamically linked libraries.  REQUIRED.
   */
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      NULL, NULL, NULL);

  if (png_ptr == NULL) {
    if (outfilename) fclose(fp);
    fprintf(stderr,"Could not create png struct\n");
    fflush(stderr);
    exit(0);
    return (-1);
  }

  /* Allocate/initialize the image information data.  REQUIRED */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    if (outfilename) fclose(fp);
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    return (-1);
  }

  /* Set error handling.  REQUIRED if you aren't supplying your own
   * error handling functions in the png_create_write_struct() call.
   */
  if (setjmp(png_jmpbuf(png_ptr))) {
    /* If we get here, we had a problem reading the file */
    if (outfilename) fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return (-1);
  }

  /* set up the output control if you are using standard C streams */
  png_init_io(png_ptr, fp);

  /* Set the image information here.  Width and height are up to 2^31,
   * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
   * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
   * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
   * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
   * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
   * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
   */
  png_set_IHDR(png_ptr, info_ptr, (int)width, (int)height, bit_depth,
    PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
    PNG_FILTER_TYPE_BASE);

  /* Optional gamma chunk is strongly suggested if you have any guess
   * as to the correct gamma of the image. */
  png_set_gAMA(png_ptr, info_ptr, gamma);

  /* Write the file header information.  REQUIRED */
  png_write_info(png_ptr, info_ptr);

  /* One of the following output methods is REQUIRED */
  // png_write_image(png_ptr, row_pointers);
  png_write_image(png_ptr, img);

  /* It is REQUIRED to call this to finish writing the rest of the file */
  png_write_end(png_ptr, info_ptr);

  /* clean up after the write, and free any memory allocated */
  png_destroy_write_struct(&png_ptr, &info_ptr);

  // close file
  if (outfilename) fclose(fp);

  // free the data array
  free_2d_array_pb(img);

  return 0;
}

/*
 * allocate memory for a two-dimensional array of png_byte
 */
png_byte** allocate_2d_array_pb(size_t nx, size_t ny, int depth) {

   size_t i;
   int bytesperpixel;
   png_byte **array;

   if (depth <= 8) bytesperpixel = 1;
   else bytesperpixel = 2;
   array = (png_byte **)malloc(nx * sizeof(png_byte *));
   array[0] = (png_byte *)malloc(bytesperpixel * nx * ny * sizeof(png_byte));

   for (i=1; i<nx; i++)
      array[i] = array[0] + i * bytesperpixel * ny;

   return(array);
}

int free_2d_array_pb(png_byte** array){
   free(array[0]);
   free(array);
   return(0);
}

