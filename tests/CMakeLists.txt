cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasotests)

configure_file(${CMAKE_SOURCE_DIR}/tests/sql/create_db.sql ${CMAKE_BINARY_DIR}/tests/create_db.sql @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/tests/sql/create_objects.sql ${CMAKE_BINARY_DIR}/tests/create_objects.sql @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/tests/sql/drop_db.sql ${CMAKE_BINARY_DIR}/tests/drop_db.sql @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/tests/sql/drop_objects.sql ${CMAKE_BINARY_DIR}/tests/drop_objects.sql @ONLY)
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

set(SERVER_KEY_PATH "${PROJECT_BINARY_DIR}/test_server.key")
set(SERVER_CERT_PATH "${PROJECT_BINARY_DIR}/test_server.csr")

aux_source_directory(${PROJECT_SOURCE_DIR} BASE_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/mocks UI_MOCK_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/admin/mocks ADMIN_MOCK_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/admin ADMIN_TEST_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/common COMMON_TEST_SOURCES)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui UI_TEST_SOURCES)

add_executable(${PROJECT_NAME} ${BASE_SOURCES} ${UI_MOCK_SOURCES} ${ADMIN_MOCK_SOURCES} ${ADMIN_TEST_SOURCES} ${COMMON_TEST_SOURCES} ${UI_TEST_SOURCES})
add_dependencies(${PROJECT_NAME} pasocommon)
add_dependencies(${PROJECT_NAME} pasoui)
add_dependencies(${PROJECT_NAME} pasoadminlib)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DSERVER_KEY_PATH="${SERVER_KEY_PATH}")
target_compile_definitions(${PROJECT_NAME} PUBLIC -DSERVER_CERT_PATH="${SERVER_CERT_PATH}")
target_link_libraries(${PROJECT_NAME} pasocommon)
target_link_libraries(${PROJECT_NAME} pasoui)
target_link_libraries(${PROJECT_NAME} pasoadminlib)
enable_coverage(${PROJECT_NAME} TRUE)

# Enable testing
enable_testing()
add_test(NAME "${PROJECT_NAME}" COMMAND ${PROJECT_NAME})

# Add custom check target that run tests because test target does not build binaries.
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    DEPENDS
    ${PROJECT_NAME}
    )

if (TARGET clean-coverage)
    add_dependencies(check clean-coverage)
    if (TARGET coverage)
        add_dependencies(coverage clean-coverage)
    endif()
endif()

# Add custom target to generate server key and certificate for testing.
add_custom_target(generate-test-keys COMMAND
    ${OPENSSL}
    req
    -x509
    -newkey rsa:2048
    -keyout ${SERVER_KEY_PATH}
    -nodes
    -days 365
    -out ${SERVER_CERT_PATH}
    -subj '/C=RS/ST=./L=./CN=.'
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    BYPRODUCTS ${SERVER_KEY_PATH} ${SERVER_CERT_PATH}
    COMMENT "Generating test server key and certificate."
    )

add_dependencies(${PROJECT_NAME} generate-test-keys)
