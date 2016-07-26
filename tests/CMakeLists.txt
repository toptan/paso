cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

configure_file(${CMAKE_SOURCE_DIR}/tests/sql/in_memory.sql ${CMAKE_BINARY_DIR}/tests/in_memory.sql @ONLY)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(common)
add_subdirectory(ui)
add_subdirectory(admin)

# Don't forget to add each test to DEPENDS section
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    DEPENDS
    testdata
    testpasodb
    testcommdata
    testcommmanager
    testmodels
    testforms
    testwidgets
    testlistadministration
    testcourseadministration
    )
