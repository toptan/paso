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

add_executable(${PROJECT_NAME} ${TEST_SOURCES} ${MOCK_SOURCES})
add_dependencies(${PROJECT_NAME} pasoui)
add_test(NAME "ui_tests" COMMAND ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} pasocommon)
target_link_libraries(${PROJECT_NAME} pasoui)
enable_coverage(${PROJECT_NAME} pasoui)
