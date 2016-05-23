cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(paso)

# Include our cmake macros and functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Utils)

# Find include files in corresponding build directories (Qt requires this)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc when needed
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run uic when needed
set(CMAKE_AUTOUIC ON)

# Tell compiler to use c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Check for the dependencies
find_package(Qt5Core    5.6 REQUIRED)
find_package(Qt5Network 5.6 REQUIRED)
find_package(Qt5Sql     5.6 REQUIRED)
find_package(Qt5Test    5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)

# Enable testing
enable_testing()

# Common PaSo library
file(GLOB COMMON_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(pasocommon STATIC ${COMMON_SOURCES})
target_link_libraries(pasocommon Qt5::Core)
target_link_libraries(pasocommon Qt5::Sql)
set(PASOCOMMON_LIBRARIES pasocommon CACHE STRING "PaSo common libraries")
set(PASOCOMMON_INCLUDE_DIRS ${PROJECT_SOURCE_DIR} CACHE STRING "PaSo commmon include directories")
enable_coverage(pasocommon)

# Common PaSo library tests
file(GLOB COMMON_TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/common/*.cpp)
add_executable(pasocommon-tests ${COMMON_TEST_SOURCES})
target_include_directories(pasocommon-tests PRIVATE ${PROJECT_SOURCE_DIR}/common)
target_link_libraries(pasocommon-tests pasocommon)
target_link_libraries(pasocommon-tests Qt5::Core)
target_link_libraries(pasocommon-tests Qt5::Sql)
target_link_libraries(pasocommon-tests Qt5::Test)
enable_coverage(pasocommon-tests pasocommon)
add_test(common-tests pasocommon-tests)
