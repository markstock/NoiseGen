# CMake input file for noisegen
#
# This file is part of NoiseGen.
# Copyright 2012 Mark J. Stock and James Sussino
#
# NoiseGen is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Foobar is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NoiseGen.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 2.6)

# common definitions
project(noisegen)
FILE(GLOB MAIN *.cpp *.c *.h *.hpp)

IF(UNIX)

	#set (CMAKE_C_FLAGS "-std=c99 -O0 -g -ggdb")
	set (CMAKE_C_FLAGS "-std=c99 -O2")
	INCLUDE(FindPNG)
	#INCLUDE(FindFFTW)
	#link_directories (/usr/lib64/libfftw3f.so)
	SET( PLATFORM_LIBS fftw3f ${PNG_LIBRARIES} m )

ELSEIF(WIN32)
	#
	# Some of the content in this section is cross-platform(such as the glob and the Find scripts)
	# and can be pushed incrementally out of the WIN32 section.
	#

	FIND_PACKAGE(Boost)
	IF (Boost_FOUND)
		INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
	ENDIF()

	INCLUDE_DIRECTORIES("./")

	INCLUDE_DIRECTORIES("C:/lib/fftw-3.3.2-dll32")
	LINK_DIRECTORIES   ("C:/lib/fftw-3.3.2-dll32")

	#INCLUDE(FindFFTW)
	# The FindPNG script doesn't appear to work on Windows...
	# Instead, I went here http://dicom.offis.de/dcmtk.php.en
	# and downloaded dcmtk-3.6.0-win32-i386-support_MD.zip
	INCLUDE_DIRECTORIES("C:/lib/libpng-1.4.3/include")
	LINK_DIRECTORIES   ("C:/lib/libpng-1.4.3/lib")
	INCLUDE_DIRECTORIES("C:/lib/zlib-1.2.5/include")
	LINK_DIRECTORIES   ("C:/lib/zlib-1.2.5/lib")

	# Compile C files as C++ in Visual Studio 9 2008
	set_source_files_properties(${MAIN} PROPERTIES LANGUAGE CXX)
	set_target_properties(PROPERTIES LINKER_LANGUAGE CXX)

	SET( PLATFORM_LIBS libfftw3f-3 libpng_d zlib_d ${PNG_LIBRARIES} )
ENDIF(UNIX)

add_executable (noisegen ${MAIN})
target_link_libraries (noisegen ${PLATFORM_LIBS})
