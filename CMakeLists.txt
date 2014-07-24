cmake_minimum_required(VERSION 2.8.11)

project(stadic)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11")
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
# The following line is only for visual studio #11 with google test (Fix later)
# add_definitions(-D_VARIADIC_MAX=10)
set(qtest_force_shared_crt ON)
add_subdirectory(gtest-1.7.0)
add_subdirectory(test)
add_subdirectory(wea)
