cmake_minimum_required(VERSION 2.8.11)

project(lib)

set(DEP_SRCS ../dependencies/jsoncpp/jsoncpp.cpp)

set(SRCS analemma.cpp
         buildingcontrol.cpp
         controlzone.cpp
         dayill.cpp
         daylight.cpp
         elecill.cpp
         functions.cpp
         filepath.cpp
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
         spacecontrol.cpp
         shadecontrol.cpp
         stadicprocess.cpp
         weatherdata.cpp
         windowgroup.cpp)

set(HDRS stadicapi.h
         sharedvector.h
         spacecontrol.h
         buildingcontrol.h
         windowgroup.h
         shadecontrol.h
         controlzone.h
         logging.h
         gridmaker.h
         photosensor.h
         processshade.h
         radfiledata.h
         radparser.h
         radprimitive.h
         geometryprimitives.h
         weatherdata.h
         dayill.h
         elecill.h
         daylight.h
         materialprimitives.h
         metrics.h
         leakcheck.h
         functions.h
         filepath.h
         analemma.h
         stadicprocess.h
         jsonobjects.h)

 add_library(stadic_core SHARED ${HDRS} ${SRCS} ${DEP_SRCS})
 add_dependencies(stadic_core boost-geometry)
