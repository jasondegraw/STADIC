cmake_minimum_required(VERSION 2.8.11)

project(lib)

include_directories("../")

set(SRCS stadiccontrol.cpp
         windowgroup.cpp
         shadecontrol.cpp
         controlzone.cpp
         logging.cpp
         gridmaker.cpp
         radfiledata.cpp
         radprimitive.cpp
         geometryprimitives.cpp
         weatherdata.cpp
         dayill.cpp
         elecill.cpp
         daylight.cpp
         materialprimitives.cpp
         leakcheck.cpp
         functions.cpp
         objects.cpp
         analemma.cpp
         process.cpp)
		 
set(HDRS stadicapi.h
         stadiccontrol.h
         windowgroup.h
         shadecontrol.h
         controlzone.h
         logging.h
         gridmaker.h
         radfiledata.h
         radprimitive.h
         geometryprimitives.h
         weatherdata.h
         dayill.h
         elecill.h
         daylight.h
         materialprimitives.h
         leakcheck.h
         functions.h
         objects.h
         analemma.h
         process.h)

add_library(stadic_core SHARED ${HDRS} ${SRCS})

target_link_libraries(stadic_core ${Boost_LIBRARIES})
