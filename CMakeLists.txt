cmake_minimum_required(VERSION 2.8.11)

project(stadic)

if(CMAKE_COMPILER_IS_GNUCXX)
  # Apparently 4.6 is the earliest version supporting nullptr and range-based for
  # https://gcc.gnu.org/projects/cxx0x.html
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "4.6.0")
    message(FATAL_ERROR "g++ versions earlier than 4.6.0 are not supported")
  endif()
  set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

if(MSVC)
  # Visual Studio compiler check, totally borrowed from OpenStudio
  # http://en.wikipedia.org/wiki/Visual_C%2B%2B#32-bit_and_64-bit_versions
  if(${CMAKE_C_COMPILER_VERSION} VERSION_LESS "18.0.21005.1")
    message(FATAL_ERROR "Visual Studio earlier than VS2013 is not supported")
  endif()
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

add_subdirectory(lib)
add_subdirectory(utilities)

# Include the tests if gtest is found
if(EXISTS ${CMAKE_SOURCE_DIR}/gtest-1.7.0/)
  if(MSVC)
    set(gtest_force_shared_crt ON)
  endif()
  add_subdirectory(gtest-1.7.0)
  add_subdirectory(test)
else()
  message(WARNING "Failed to find gtest 1.7.0, tests will not be built")
endif()
