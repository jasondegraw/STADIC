import os
import shutil
import subprocess
import argparse
import datetime

#
# Set up optional arguments
#
parser = argparse.ArgumentParser()
parser.add_argument("--destination",
                    default = '../stadic-export',
                    help="directory to write exported files to")
parser.add_argument("--git",
                    default = 'git',
                    #'C:\\Program Files (x86)\\Git\\bin\\git.exe',
                    help="full path to git executable")
parser.add_argument("--boost-prefix",
                    dest = 'prefix',
                    default = '',
                    help="prefix for hardcoded addresses to locate boost")
args = parser.parse_args()

destination = args.destination
gitcmd = args.git
prefix = args.prefix

print('Exporting STADIC to "%s"...' % destination)

if os.path.exists(destination):
    exit('Destination path "%s" already exists!' % destination)

os.makedirs(destination)
print("Copying dependencies...")
#
# Copy all dependencies
#
shutil.copytree('dependencies', destination + '/dependencies')
#
# Copy all docs?
#

#
# Prune lib down to only the files we absolutely need
#
print("Copying library source files...")
os.makedirs(destination+'/lib')
cmakeTxt = """cmake_minimum_required(VERSION 2.8.11)

project(lib)

set(DEP_SRCS ../dependencies/jsoncpp/jsoncpp.cpp)

set(SRCS %s)

set(HDRS %s)

add_library(stadic_core SHARED ${HDRS} ${SRCS} ${DEP_SRCS})
add_dependencies(stadic_core boost-geometry)

install(TARGETS stadic_core
	LIBRARY DESTINATION bin)"""

filesBoth = [#'analemma',
             #'buildingcontrol',
             #'controlzone',
             #'dayill',
             #'daylight',
             #'elecill',
             'functions',
             'filepath',
             'geometryprimitives',
             'gridmaker',
             #'jsonobjects',
             #'leakcheck',
             'logging',
             'materialprimitives',
             #'metrics',
             #'photosensor',
             #'processshade',
             'radfiledata',
             'radparser',
             'radprimitive',
             #'spacecontrol',
             #'shadecontrol',
             'stadicprocess',
             #'weatherdata',
             #'windowgroup'
             ]

filesHeader = ['stadicapi.h', 'sharedvector.h']

src = ['%s.cpp' % el for el in filesBoth]

hdr = filesHeader + ['%s.h' % el for el in filesBoth]

# Write the CMake file
fp = open(destination+'/lib/CMakeLists.txt', 'w')
fp.write(cmakeTxt % (' '.join(src), ' '.join(hdr)))
fp.close()

# Copy the files
for file in (src+hdr):
    shutil.copy('lib/'+file, destination+'/lib/'+file)

#
# Copy utilities that are to be built
#
print("Copying utility source files...")
os.makedirs(destination+'/utilities')
cmakeTxt = """cmake_minimum_required(VERSION 2.8.11)

project(utilities)

include_directories(../lib)

"""

exeTxt = """add_executable(%s %s.cpp)
target_link_libraries(%s stadic_core)
install(TARGETS %s
	RUNTIME DESTINATION bin)

"""

utils = ['dxgridmaker']

for exe in utils:
    cmakeTxt += (exeTxt % (exe, exe, exe, exe))

# Write the CMake file
fp = open(destination+'/utilities/CMakeLists.txt', 'w')
fp.write(cmakeTxt)
fp.close()

# Copy the files
src = ['%s.cpp' % el for el in utils]
for file in src:
    shutil.copy('utilities/'+file, destination+'/utilities/'+file)

#
# Write top level files
#
print("Writing top level CMakeLists.txt and README files...")
cmakeTxt = """cmake_minimum_required(VERSION 2.8.11)

project(stadic)

if(CMAKE_COMPILER_IS_GNUCXX)
  # Apparently 4.6 is the earliest version supporting nullptr and range-based for
  # https://gcc.gnu.org/projects/cxx0x.html
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "4.6.0")
    message(FATAL_ERROR "g++ versions earlier than 4.6.0 are not supported")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(MSVC)
  # Visual Studio compiler check, totally borrowed from OpenStudio
  # http://en.wikipedia.org/wiki/Visual_C%%2B%%2B#32-bit_and_64-bit_versions
  if(${CMAKE_C_COMPILER_VERSION} VERSION_LESS "18.0.21005.1")
    message(FATAL_ERROR "Visual Studio earlier than VS2013 is not supported")
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # This check for clang seems to work, at least on Ubuntu. Not that much 
  # else works on Ubuntu: had to add -I/usr/include/c++/4.8/ and 
  # -I/usr/include/x86_64-linux-gnu/c++/4.8/ to get it to compile.
  # Mac works as well!
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Boost
include(ExternalProject)
ExternalProject_Add(boost-geometry
  URL ${CMAKE_SOURCE_DIR}/%sdependencies/boost-geometry-1_57.tar.gz
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")
set(Boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/%sboost-geometry-prefix/src/boost-geometry)
include_directories(${Boost_INCLUDE_DIR})

# JSON Dependency
include_directories("dependencies/jsoncpp")

add_subdirectory(lib)
add_subdirectory(utilities)
""" % (prefix, prefix)

fp = open(destination+'/CMakeLists.txt', 'w')
fp.write(cmakeTxt)
fp.close()

#
# Try to get the git SHA of this commit
#
gitSha = 'UNKNOWN'
try:
    if os.path.exists(gitcmd):
        output = subprocess.Popen([gitcmd, 'rev-parse', '--short', 'HEAD'],
                                  stdout=subprocess.PIPE).communicate()[0]
        gitSha = output.decode('utf-8').strip()
except:
    gitSha = 'UNKNOWN'

readmeTxt = """STADIC
======

Simulation Tool for Architectural Daylighting and Integrated Controls

Developed by Rick Mistrick, Craig Casey, and Jason DeGraw at Penn State

Description
-----------

STADIC is based upon the Radiance ray-tracing software system. This is
a subset created from git commit %s on %s with export.py.
""" % (gitSha, datetime.datetime.now().strftime('%c'))

fp = open(destination+'/README.txt', 'w')
fp.write(readmeTxt)
fp.close()
print("Export complete.")
