cmake_minimum_required(VERSION 2.8.11)

project(stadic)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

add_subdirectory(lib)
add_subdirectory(test)
