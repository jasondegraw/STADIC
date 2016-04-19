cmake_minimum_required(VERSION 2.8.11)

project(test)

# Macro to make test creation shorter
macro(create_test target)
  add_executable(${target} ${target}.cpp)
  target_link_libraries(${target} stadic_core gtest gtest_main)
  add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}> WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endmacro(create_test)

include_directories(../lib ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

create_test(controltests)
# This is suboptimal, but it works
add_custom_command(TARGET controltests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/control.json $<TARGET_FILE_DIR:controltests>)

create_test(weathertests)
add_custom_command(TARGET weathertests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/USA_PA_Lancaster.AP.725116_TMY3.epw $<TARGET_FILE_DIR:weathertests>)
add_custom_command(TARGET weathertests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/LancasterTMY.csv $<TARGET_FILE_DIR:weathertests>)

create_test(sensortests)

create_test(primitivetests)

create_test(radfiletests)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/material.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/simple.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/supportedprimitives.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/badplasticmaterial.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/badmetalmaterial.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/badglassmaterial.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/badtransmaterial.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/badpolygongeometry.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/empty.rad $<TARGET_FILE_DIR:radfiletests>)
add_custom_command(TARGET radfiletests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/materialsingleline.rad $<TARGET_FILE_DIR:radfiletests>)
#This was in here previously
#                   ${CMAKE_SOURCE_DIR}/test/resources/daylightcase1/rad/mat1.rad $<TARGET_FILE_DIR:radfiletests>)

create_test(leaktests)
add_custom_command(TARGET leaktests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/simplehole.rad $<TARGET_FILE_DIR:leaktests>)

create_test(functiontests)

create_test(radparsertests)

create_test(filepathtests)

create_test(processtests)

create_test(analemmatests)

add_executable(testprogram testprogram.cpp)

create_test(gridtests)
add_custom_command(TARGET gridtests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/complicated.rad $<TARGET_FILE_DIR:gridtests>)
add_custom_command(TARGET gridtests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/diamond.rad $<TARGET_FILE_DIR:gridtests>)

create_test(daylighttest)
add_custom_command(TARGET daylighttest POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/test/resources/daylightcase1/ $<TARGET_FILE_DIR:daylighttest>/daylightcase1/)


set(RESOURCES resources/simple.rad
              resources/simplehole.rad
              resources/material.rad
              resources/supportedprimitives.rad
              resources/badplasticmaterial.rad
              badmetalmaterial.rad
              resources/badglassmaterial.rad
              resources/badtransmaterial.rad
              resources/badpolygongeometry.rad
              resources/daylightcase1/JSON_TEST1.txt)
