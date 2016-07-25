cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(testui)

# PaSo common library tests include dirs and libraries
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/ui)
include_directories(${CMAKE_SOURCE_DIR}/admin)
link_libraries(Qt5::Core)
link_libraries(Qt5::Network)
link_libraries(Qt5::Sql)
link_libraries(Qt5::Test)
link_libraries(Qt5::Widgets)
link_libraries(pasoadminlib)
link_libraries(pasoui)
link_libraries(pasocommon)

add_executable(testlistadministration testlistadministration.cpp)
add_dependencies(testlistadministration pasoadminlib)
add_dependencies(testlistadministration pasoui)
add_test(NAME "list_administration_tests" COMMAND testlistadministration)
enable_coverage(testlistadministration pasoadminlib pasoui pasocommon)

