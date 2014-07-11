cmake_minimum_required(VERSION 2.8.11)

project(wea)

enable_testing(true)

include_directories(../lib)

add_executable(make_wea make_wea.cpp)

target_link_libraries(make_wea stadic_core)
