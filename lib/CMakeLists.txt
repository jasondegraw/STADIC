cmake_minimum_required(VERSION 2.8.11)

project(lib)

set(SRCS stadiccontrol.cpp
         windowgroup.cpp
         shadecontrol.cpp
         controlzone.cpp)
		 
set(HDRS stadiccontrol.h
         windowgroup.h
         shadecontrol.h
         controlzone.h)

add_library(stadic_core SHARED ${HDRS} ${SRCS})

target_link_libraries(stadic_core Qt5::Core)
