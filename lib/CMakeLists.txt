cmake_minimum_required(VERSION 2.8.11)

project(lib)

set(DEP_SRCS ../dependencies/jsoncpp/jsoncpp.cpp)

set(SRCS spacecontrol.cpp
         buildingcontrol.cpp
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
         filepath.cpp
         analemma.cpp
         stadicprocess.cpp
         jsonobjects.cpp)
		 
set(HDRS stadicapi.h
         sharedvector.h
         spacecontrol.h
         buildingcontrol.h
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
         filepath.h
         analemma.h
         stadicprocess.h
         jsonobjects.h)

 add_library(stadic_core SHARED ${HDRS} ${SRCS} ${DEP_SRCS})
 add_dependencies(stadic_core boost-geometry)
