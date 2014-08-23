cmake_minimum_required(VERSION 2.8.11)

project(lib)

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
         materialprimitives.cpp)
		 
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
         materialprimitives.h)

add_library(stadic_core SHARED ${HDRS} ${SRCS})
target_link_libraries(stadic_core Qt5::Widgets)
install(TARGETS stadic_core DESTINATION lib)
