cmake_minimum_required(VERSION 2.8.11)

project(test)

enable_testing(true)

include_directories(../lib ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(controltests controltests.cpp)
add_test(controltests controltests)
target_link_libraries(controltests stadic_core gtest gtest_main)
# This is suboptimal, but it works
add_custom_command(TARGET controltests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/control.json $<TARGET_FILE_DIR:controltests>)

add_executable(weathertests weathertests.cpp)
add_test(weathertests weathertests)
target_link_libraries(weathertests stadic_core gtest gtest_main)
add_custom_command(TARGET weathertests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/USA_PA_Lancaster.AP.725116_TMY3.epw $<TARGET_FILE_DIR:weathertests>)
add_custom_command(TARGET weathertests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/LancasterTMY.csv $<TARGET_FILE_DIR:weathertests>)
add_executable(sensortests sensortests.cpp)
add_test(sensortests sensortests)
target_link_libraries(sensortests stadic_core gtest gtest_main
)
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

add_executable(leaktests leaktests.cpp)
add_test(leaktests leaktests)
target_link_libraries(leaktests stadic_core gtest gtest_main)
add_custom_command(TARGET leaktests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/simplehole.rad $<TARGET_FILE_DIR:leaktests>)

add_executable(functiontests functiontests.cpp)
add_test(functiontests functiontests)
target_link_libraries(functiontests stadic_core gtest gtest_main)

add_executable(radparsertests radparsertests.cpp)
add_test(radparsertests radparsertests)
target_link_libraries(radparsertests stadic_core gtest gtest_main)

add_executable(filepathtests filepathtests.cpp)
add_test(filepathtests filepathtests)
target_link_libraries(filepathtests stadic_core gtest gtest_main)

add_executable(processtests processtests.cpp)
add_test(processtests processtests)
target_link_libraries(processtests stadic_core gtest gtest_main)

add_executable(analemmatests analemmatests.cpp)
add_test(analemmatests analemmatests)
target_link_libraries(analemmatests stadic_core gtest gtest_main)

add_executable(processshadetests shadeprocesstest.cpp)
add_test(processshadetests processshadetests)
target_link_libraries(processshadetests stadic_core gtest gtest_main)

add_executable(illuminancetests illuminancetests.cpp)
add_test(illuminancetests illuminancetests)
target_link_libraries(illuminancetests stadic_core gtest gtest_main)
add_custom_command(TARGET illuminancetests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/10PointsTimeBased.ill $<TARGET_FILE_DIR:illuminancetests>)
add_custom_command(TARGET illuminancetests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/test/resources/10PointsNonTimeBased.ill $<TARGET_FILE_DIR:illuminancetests>)

add_executable(testprogram testprogram.cpp)

add_executable(gridtests gridtests.cpp)
add_test(gridtests gridtests)
target_link_libraries(gridtests stadic_core gtest gtest_main)
add_custom_command(TARGET gridtests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/complicated.rad $<TARGET_FILE_DIR:gridtests>)
add_custom_command(TARGET gridtests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/test/resources/diamond.rad $<TARGET_FILE_DIR:gridtests>)

add_executable(daylighttest daylighttest.cpp)
add_test(daylighttest daylighttest.cpp)
target_link_libraries(daylighttest stadic_core gtest gtest_main)
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
