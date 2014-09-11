cmake_minimum_required(VERSION 2.8.11)

project(test)

enable_testing(true)

include_directories(../lib ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

#qt5_add_resources(QRCS TestResource.qrc)

add_executable(controltests controltests.cpp)
add_test(controltests controltests)
target_link_libraries(controltests stadic_core gtest gtest_main)
# This is suboptimal, but it works
add_custom_command(TARGET controltests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/TestSControl.json $<TARGET_FILE_DIR:controltests>)

add_executable(weathertests weathertests.cpp)
add_test(weathertests weathertests)
target_link_libraries(weathertests stadic_core gtest gtest_main)
add_custom_command(TARGET weathertests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/USA_PA_Lancaster.AP.725116_TMY3.epw $<TARGET_FILE_DIR:weathertests>)

add_executable(primitivetests primitivetests.cpp)
add_test(primitivetests primitivetests)
target_link_libraries(primitivetests stadic_core gtest gtest_main)

add_executable(radfiletests radfiletests.cpp)
add_test(radfiletests radfiletests)
target_link_libraries(radfiletests stadic_core gtest gtest_main)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/material.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/Simple.rad $<TARGET_FILE_DIR:radfiletests>)

add_executable(leaktests leaktests.cpp)
add_test(leaktests leaktests)
target_link_libraries(leaktests stadic_core gtest gtest_main)
add_custom_command(TARGET leaktests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/SimpleHole.rad $<TARGET_FILE_DIR:leaktests>)
add_custom_command(TARGET leaktests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/material.rad $<TARGET_FILE_DIR:leaktests>)

add_executable(functiontests functiontests.cpp)
add_test(functiontests functiontests)
target_link_libraries(functiontests stadic_core gtest gtest_main)

add_executable(objectstests objectstests.cpp)
add_test(objectstests objectstests)
target_link_libraries(objectstests stadic_core gtest gtest_main)

set(RESOURCES resources/SimpleHole.rad
			  resources/standard/SimpleStd.json
			  resources/standard/data/Lancaster.wea
			  resources/standard/rad/material.rad
			  resources/standard/rad/Simple.rad
			  resources/standard/rad/SimpleShadesFull.rad
			  resources/standard/rad/SimpleShadesHalf.rad)
