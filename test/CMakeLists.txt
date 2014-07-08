cmake_minimum_required(VERSION 2.8.11)

project(test)

add_executable(controltests controltests.cpp)

target_link_libraries(controltests stadic_core)
