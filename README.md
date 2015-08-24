# NoiseGen
Command-line tool to generate 1D, 2D, or 3D noise textures

----------------------------------------------

This file is part of NoiseGen.
Copyright 2012,5 Mark J. Stock and James Sussino

NoiseGen is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

NoiseGen is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with NoiseGen.  If not, see <http://www.gnu.org/licenses/>.

----------------------------------------------

## Installation and usage

Requires CMake, FFTW, libpng, and Boost to build. Install these on Fedora/RPM systems with:

    sudo yum install cmake libpng-devel fftw-devel boost-devel boost-random

Then, clone this repository:

    git clone https://github.com/markstock/NoiseGen.git

Make a build subdirectory and run cmake:

    cd NoiseGen
    mkdir build
    cd build
    cmake ..
    make

To run the resulting program and see a list of options, run

    ./noisegen -h

Here are some sample command-lines:

    noisegen -d 2 -n 512 512 -o out01.png
    noisegen -d 2 -n 512 512 -g -o out02.png
    noisegen -d 2 -n 512 512 -pink -o out03.png
    noisegen -d 2 -n 512 512 -e -0.5 -o out04.png
    noisegen -d 2 -n 512 512 -e -1.5 -o out05.png
    noisegen -d 2 -n 512 512 -e -1.5 -seed 134676 -o out06.png
    noisegen -d 2 -n 512 512 -e -0.5 -p 0.1 -1 0 0.05 10.0 -p 0.8 -0.2 0 0.05 10.0 -p 0.7 0.1 0 0.05 10.0 -o out15.png
    noisegen -d 2 -n 512 512 -e -0.5 -p 0.1 -1 0 0.05 3.0 -p 0.8 -0.2 0 0.05 3.0 -p 0.7 0.1 0 0.05 3.0 -o out16.png
    noisegen -d 2 -n 512 512 -e -1.2 -p 0.1 -1 0 0.05 3.0 -p 0.8 -0.2 0 0.05 3.0 -p 0.7 0.1 0 0.05 3.0 -o out17.png
    noisegen -d 2 -n 100 300 -e -0.5 -p 0.7 0.7 0 0.05 10.0 -o out18.png
    noisegen -d 2 -n 700 300 -e -0.5 -p 0.7 0.7 0 0.05 10.0 -o out19.png
    noisegen -d 2 -n 500 300 -e -0.5 -p 0.7 0.7 0 0.05 10.0 -p 0.1 1.0 0 0.05 10.0 -o out20.png
    noisegen -d 2 -n 5000 3000 -pink -p 0.7 0.7 0 0.05 10.0 -p 0.1 1.0 0 0.05 5.0 -g -o out23.png
    noisegen -d 2 -n 5000 3000 -red -p 0.7 0.7 0 0.05 10.0 -p 0.1 1.0 0 0.05 5.0 -g -o out24.png
    noisegen -d 2 -n 5000 3000 -white -p 0.7 0.7 0 0.05 10.0 -p 0.1 1.0 0 0.05 5.0 -g -o out25.png

If you have any questions or encounter any problems, please contact
the authors (we're really nice people) at markjstock@gmail.com


----------------------------------------------

## Developer information

#### Still To Do

* Test code on non-cubic domains
* Consider outputting CDF (n-dim), WAV (1-dim), APNG (3-d) files
  (APNG is http://www.linuxfromscratch.org/blfs/view/svn/general/libpng.html)
* Normalize output somehow (make this a command-line option)
* Consider DICOM for 3D data, there are free viewers out there!
  (http://dicomlib.swri.ca/dicomlib.html for a C++ library)
* Use C++11, which has a native RNG with capabilities similar to boost

#### Done

* Add brick-of-shorts to supported output
* Incorporate Jim's changes to the CMakeLists.txt and reply to him
* Add CL options to paint a streak on the frequency plot (need direction, strength, width)
* Test code on non-square domains - was broken
* Try out the 3D BOBs on my hillslope evolver (needs vector data)
* Try out 2D streaks on the erosion solver - not great
* Try to add streaks on the frequency image before reconstruction
* Add switch in rng.cpp for boost version number (1.46 has different header file names)
* Is there a BOB library? what do I use for the landscape program?
* Finish 2d FFT work
* Link in PNG libs for 1d, 2d output
* Allow use of better RNGs (boost.random is awesome!)
* Put those into a random.c file? no, rng.cpp


