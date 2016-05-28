cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(testcommon)

# PaSo common library tests include dirs and libraries
include_directories(${CMAKE_SOURCE_DIR}/common)
link_libraries(Qt5::Core)
link_libraries(Qt5::Sql)
link_libraries(Qt5::Test)

# Adding tests
enable_testing()

add_executable(testdata testdata.cpp)
target_link_libraries(testdata pasocommon)
enable_coverage(testdata pasocommon)

add_executable(testpasodb testpasodb.cpp)
target_link_libraries(testpasodb pasocommon)
enable_coverage(testpasodb pasocommon)

add_executable(testcommdata testcommdata.cpp)
target_link_libraries(testcommdata pasocommon)
enable_coverage(testcommdata pasocommon)
