cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasosimulator)

# Administration module
file(GLOB SIMULATOR_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SIMULATOR_SOURCES} ${CMAKE_BINARY_DIR}/translations.qrc ${CMAKE_BINARY_DIR}/certs.qrc)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/common)
add_dependencies(${PROJECT_NAME} pasocommon)
add_dependencies(${PROJECT_NAME} translations)
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} pasocommon)

set(SIMULATOR_KEYS_DIR ${PROJECT_BINARY_DIR})
set(SIMULATOR_KEY_PATH ${SIMULATOR_KEYS_DIR}/simulator.key)
set(SIMULATOR_CERT_PATH ${SIMULATOR_KEYS_DIR}/simulator.csr)

add_dependencies(${PROJECT_NAME} generate-simulator-keys)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
