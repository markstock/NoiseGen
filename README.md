# NoiseGen
Command-line tool to generate 1D, 2D, or 3D noise textures
----------------------------------------------

This file is part of NoiseGen.
Copyright 2012 Mark J. Stock and James Sussino

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


Requires CMake, FFTW, libpng, and Boost to build. Install these on Fedora/RPM systems with:

    sudo yum install cmake libpng-devel fftw-devel

Then, clone this repository:

    git clone https://github.com/markstock/NoiseGen.git

Make a build subdirectory and run cmake:

    cd NoiseGen
    mkdir build
    cd build
    cmake ..
    make

To run the resulting software, and see a list of options, run

    ./noisegen -h

If you have any questions or encounter any problems, please contact
the authors (we're really nice people) at mstock@umich.edu.

2012-09-26

