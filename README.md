STADIC
======

Simulation Tool for Architectural Daylighting and Integrated Controls

Developed by Rick Mistrick, Craig Casey, and Jason DeGraw at Penn State

Description
-----------

STADIC is based upon the Radiance ray-tracing software system.

Programs
--------

STADIC is made up of a number of subprograms, including:

* dxanalemma - generate the necessary files for a five phase analysis
* dxdaylight - simulate the daylight for a space model
* dxgridmaker - create an analysis grid from Radiance geometry primitives
* dxleakcheck - test whether a Radiance model represents an enclosed space
	
Building
--------

STADIC is built with CMake (version 2.8 or higher) and is known to compile
with the following compilers:

* GCC 4.8.2 (Ubuntu 14.04)
* MinGW GCC 4.8.2 (32 bit)
* Microsoft Visual Studio 2013
* Clang 3.5 (XCode 6.1)

To build the software, the general steps are as follows:

1. Get the source code. Clone the GitHub repo or download the code as a
compressed file and uncompress it.

2. (Optional) Get the Google test framework. Download the source code of the
Google test framework version 1.7.0 (https://code.google.com/p/googletest) and 
uncompress it in the source code directory as gtest-1.7.0.

3. Run CMake to create project. For IDEs, the details depend upon the IDE that
is being used.

    a. Visual Studio or XCode: Run the CMake GUI to generate the build files,
    then open the .sln or .xcode file with the IDE.

    b. Qt Creator: Use the File/Open File or Project... menu item to select 
    STADIC's CMakeLists.txt file, then run CMake.
    
4. Build the program. If CMake has found Google test, then the tests will be
built. Otherwise, only the programs will be built.

Acknowledgements
----------------

The development of this software is supported by the Consortium for Building Energy 
Innovation (CBEI) sponsored  by the U.S. Department of Energy under Award Number
DE-EE0004261.
