cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasotests)

configure_file(${CMAKE_SOURCE_DIR}/tests/sql/in_memory.sql ${CMAKE_BINARY_DIR}/tests/in_memory.sql @ONLY)
file(GLOB TEST_FILES LIST_DIRECTORIES false RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/files/*)
foreach(_TEST_FILE ${TEST_FILES})
    configure_file(${PROJECT_SOURCE_DIR}/${_TEST_FILE} ${PROJECT_BINARY_DIR}/${_TEST_FILE})
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/ui)
include_directories(${CMAKE_SOURCE_DIR}/admin)

include_directories(${PROJECT_SOURCE_DIR}/admin)
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/ui)
include_directories(${PROJECT_SOURCE_DIR}/ui/mocks)

link_libraries(Qt5::Core)
link_libraries(Qt5::Network)
link_libraries(Qt5::Sql)
link_libraries(Qt5::Test)

aux_source_directory(${PROJECT_SOURCE_DIR} BASE_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/mocks MOCK_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/admin ADMIN_TEST_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/common COMMON_TEST_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui UI_TEST_SOURCES)

add_executable(${PROJECT_NAME} ${BASE_SOURCES} ${MOCK_SOURCES} ${ADMIN_TEST_SOURCES} ${COMMON_TEST_SOURCES} ${UI_TEST_SOURCES})
add_dependencies(${PROJECT_NAME} pasocommon)
add_dependencies(${PROJECT_NAME} pasoui)
add_dependencies(${PROJECT_NAME} pasoadminlib)
target_link_libraries(${PROJECT_NAME} pasocommon)
target_link_libraries(${PROJECT_NAME} pasoui)
target_link_libraries(${PROJECT_NAME} pasoadminlib)
enable_coverage(${PROJECT_NAME} pasocommon pasoui pasoadminlib)

# Enable testing
enable_testing()
add_test(NAME "${PROJECT_NAME}" COMMAND ${PROJECT_NAME})

# Don't forget to add each test to DEPENDS section
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    DEPENDS
    ${PROJECT_NAME}
    )
