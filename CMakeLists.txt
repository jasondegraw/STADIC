cmake_minimum_required(VERSION 2.8.11)

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
  # http://en.wikipedia.org/wiki/Visual_C%2B%2B#32-bit_and_64-bit_versions
  if(${CMAKE_C_COMPILER_VERSION} VERSION_LESS "18.0.21005.1")
    message(FATAL_ERROR "Visual Studio earlier than VS2013 is not supported")
  endif()
elseif(Apple)
  # The checks to determine if the compiler is clang are all rather sketchy,
  # and maybe this should go before the g++ test above to get things right.
  # But for now skip the checks and assume everything will be Ok.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Boost
set(USE_INSTALLED_BOOST OFF CACHE BOOL "Look for installed Boost")
if(USE_INSTALLED_BOOST)
  find_package(Boost 1.57 REQUIRED)
else()
  include(ExternalProject)
  ExternalProject_Add(boost-geometry
    URL ${CMAKE_SOURCE_DIR}/dependencies/boost-geometry.tar.gz
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")
  set(Boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/boost-geometry-prefix/src/boost-geometry)
endif()
include_directories(${Boost_INCLUDE_DIR})

# JSON Dependency
include_directories("dependencies/jsoncpp")

add_subdirectory(lib)
add_subdirectory(utilities)

# Include the tests if gtest is found
if(EXISTS ${CMAKE_SOURCE_DIR}/gtest-1.7.0/)
  if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use msvcrt.dll")
  endif()
  add_subdirectory(gtest-1.7.0)
  add_subdirectory(test)
else()
  message(WARNING "Failed to find gtest 1.7.0, tests will not be built")
endif()
