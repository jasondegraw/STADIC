cmake_minimum_required(VERSION 2.8.11)

project(lib)

set(DEP_SRCS ../dependencies/jsoncpp/jsoncpp.cpp)

set(SRCS analemma.cpp
         buildingcontrol.cpp
         controlzone.cpp
         dayill.cpp
         daylight.cpp
         elecill.cpp
         electriclight.cpp
         filepath.cpp
         functions.cpp
         geometryprimitives.cpp
         gridmaker.cpp
         jsonobjects.cpp
         leakcheck.cpp
         logging.cpp
         materialprimitives.cpp
         metrics.cpp
         photosensor.cpp
         processshade.cpp
         radfiledata.cpp
         radparser.cpp
         radprimitive.cpp
         shadecontrol.cpp
         spacecontrol.cpp
         stadicprocess.cpp
         weatherdata.cpp
         windowgroup.cpp)

set(HDRS analemma.h
         buildingcontrol.h
         controlzone.h
         dayill.h
         daylight.h
         elecill.h
         electriclight.h
         filepath.h
         functions.h
         geometryprimitives.h
         gridmaker.h
         jsonobjects.h
         leakcheck.h
         logging.h
         materialprimitives.h
         metrics.h
         photosensor.h
         processshade.h
         radfiledata.h
         radparser.h
         radprimitive.h
         shadecontrol.h
         sharedvector.h
         spacecontrol.h
         stadicapi.h
         stadicprocess.h
         weatherdata.h
         windowgroup.h)

 add_library(stadic_core SHARED ${HDRS} ${SRCS} ${DEP_SRCS})
 add_dependencies(stadic_core boost-geometry)
