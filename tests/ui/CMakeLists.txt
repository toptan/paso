cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(testui)

# PaSo common library tests include dirs and libraries
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/ui)
link_libraries(Qt5::Core)
link_libraries(Qt5::Network)
link_libraries(Qt5::Sql)
link_libraries(Qt5::Test)
link_libraries(Qt5::Widgets)

aux_source_directory(${PROJECT_SOURCE_DIR}/mocks MOCK_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR} TEST_SOURCES)

add_executable(testforms testforms.cpp ${MOCK_SOURCES})
add_dependencies(testforms pasoui)
target_link_libraries(testforms pasocommon)
target_link_libraries(testforms pasoui)
add_test(NAME "forms_tests" COMMAND testforms)
enable_coverage(testforms pasocommon pasoui)

add_executable(testwidgets testwidgets.cpp ${MOCK_SOURCES})
add_dependencies(testwidgets pasoui)
target_link_libraries(testwidgets pasocommon)
target_link_libraries(testwidgets pasoui)
add_test(NAME "widgets_tests" COMMAND testwidgets)
enable_coverage(testwidgets pasocommon pasoui)
