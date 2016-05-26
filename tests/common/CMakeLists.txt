cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(testcommon)

# PaSo common library tests include dirs and libraries
include_directories(${CMAKE_SOURCE_DIR}/common)
link_libraries(Qt5::Core)
link_libraries(Qt5::Sql)
link_libraries(Qt5::Test)

# Data test
add_executable(${PROJECT_NAME} main.cpp testdata.cpp testpasodb.cpp)
target_link_libraries(${PROJECT_NAME} pasocommon)

# Enabling code coverage
enable_coverage(${PROJECT_NAME} pasocommon)

# Adding tests
enable_testing()
add_test(common-tests ${PROJECT_NAME})
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -V DEPENDS ${PROJECT_NAME})
