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
find_package(Qt5Core    5.5 REQUIRED)
find_package(Qt5Network 5.5 REQUIRED)
find_package(Qt5Sql     5.5 REQUIRED)
find_package(Qt5Test    5.5 REQUIRED)
find_package(Qt5Widgets 5.5 REQUIRED)

# Add subdirectories
add_subdirectory(admin)
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(tests)
