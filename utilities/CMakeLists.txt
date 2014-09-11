cmake_minimum_required(VERSION 2.8.11)

project(utilities)

enable_testing(true)

include_directories(../lib)

add_executable(dxmakewea dxmakewea.cpp)
target_link_libraries(dxmakewea stadic_core)

add_executable(dxgridmaker dxgridmaker.cpp)
target_link_libraries(dxgridmaker stadic_core)

add_executable(dxleakcheck dxleakcheck.cpp)
target_link_libraries(dxleakcheck stadic_core)

add_executable(dxdaylight dxdaylight.cpp)
target_link_libraries(dxdaylight stadic_core)
