cmake_minimum_required(VERSION 2.8.11)

project(test)

enable_testing(true)

include_directories(../lib ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

qt5_add_resources(QRCS TestResource.qrc)

add_executable(controltests controltests.cpp ${QRCS})
add_test(controltests controltests)
target_link_libraries(controltests stadic_core gtest gtest_main)

add_executable(weathertests weathertests.cpp ${QRCS})
add_test(weathertests weathertests)
target_link_libraries(weathertests stadic_core gtest gtest_main)

add_executable(primitivetests primitivetests.cpp ${QRCS})
add_test(primitivetests primitivetests)
target_link_libraries(primitivetests stadic_core gtest gtest_main)

add_executable(radfiletests radfiletests.cpp ${QRCS})
add_test(radfiletests radfiletests)
target_link_libraries(radfiletests stadic_core gtest gtest_main)
