cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(testcommon)

# PaSo common library tests include dirs and libraries
include_directories(${CMAKE_SOURCE_DIR}/common)
link_libraries(Qt5::Core)
link_libraries(Qt5::Network)
link_libraries(Qt5::Sql)
link_libraries(Qt5::Test)
link_libraries(Qt5::Widgets)

configure_file(${PROJECT_SOURCE_DIR}/sql/in_memory.sql ${PROJECT_BINARY_DIR}/in_memory.sql @ONLY)

# Adding tests
enable_testing()

add_executable(testdata testdata.cpp)
add_test(NAME "Data_tests" COMMAND testdata)
target_link_libraries(testdata pasocommon)
enable_coverage(testdata pasocommon)

add_executable(testpasodb testpasodb.cpp)
add_test(NAME "Database_tests" COMMAND testpasodb)
target_link_libraries(testpasodb pasocommon)
enable_coverage(testpasodb pasocommon)

add_executable(testcommdata testcommdata.cpp)
add_test(NAME "Comm_data_tests" COMMAND testcommdata)
target_link_libraries(testcommdata pasocommon)
enable_coverage(testcommdata pasocommon)

add_executable(testcommmanager testcommmanager.cpp)
add_test(NAME "Communication_tests" COMMAND testcommmanager)
target_link_libraries(testcommmanager pasocommon)
enable_coverage(testcommmanager pasocommon)

add_executable(testmodels testmodels.cpp)
add_test(NAME "Model_tests" COMMAND testmodels)
target_link_libraries(testmodels pasocommon)
enable_coverage(testmodels pasocommon)

add_executable(testwidgets testwidgets.cpp)
add_test(NAME "Widgets_tests" COMMAND testwidgets)
target_link_libraries(testwidgets pasocommon)
enable_coverage(testwidgets pasocommon)

# Don't forget to add each test to DEPENDS section
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    DEPENDS
    testdata
    testpasodb
    testcommdata
    testcommmanager
    testmodels
    testwidgets
    )
