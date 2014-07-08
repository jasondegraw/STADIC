cmake_minimum_required(VERSION 2.8.11)

project(test)

enable_testing(true)

include_directories(../lib ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

qt5_add_resources(QRCS TestResource.qrc)

add_executable(controltests controltests.cpp ${QRCS})

add_test(controltests controltests)

target_link_libraries(controltests stadic_core )#gtest gtest_main)
