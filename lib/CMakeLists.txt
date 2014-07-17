cmake_minimum_required(VERSION 2.8.11)

project(lib)

set(SRCS stadiccontrol.cpp
         windowgroup.cpp
         shadecontrol.cpp
         controlzone.cpp
         logging.cpp
         gridmaker.cpp
         radgeometry.cpp
         weatherdata.cpp
         dayill.cpp
         elecill.cpp)
		 
set(HDRS stadiccontrol.h
         windowgroup.h
         shadecontrol.h
         controlzone.h
         logging.h
         gridmaker.h
         radgeometry.h
         weatherdata.h
         dayill.h
         elecill.h)

add_library(stadic_core SHARED ${HDRS} ${SRCS})

target_link_libraries(stadic_core Qt5::Widgets)

add_executable(grid_maker grid_maker.cpp)

target_link_libraries(grid_maker stadic_core)
