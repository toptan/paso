cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasoui)

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
find_package(Qt5Core       5.5 REQUIRED)
find_package(Qt5Concurrent 5.5 REQUIRED)
find_package(Qt5Network    5.5 REQUIRED)
find_package(Qt5Sql        5.5 REQUIRED)
find_package(Qt5Test       5.5 REQUIRED)
find_package(Qt5Widgets    5.5 REQUIRED)

# PaSo UI library
file(GLOB COMMON_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(${PROJECT_NAME} STATIC ${COMMON_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Concurrent)
target_link_libraries(${PROJECT_NAME} Qt5::Network)
target_link_libraries(${PROJECT_NAME} Qt5::Sql)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} pasocommon)
add_dependencies(${PROJECT_NAME} translations)
add_dependencies(${PROJECT_NAME} pasocommon)
enable_coverage(${PROJECT_NAME})
