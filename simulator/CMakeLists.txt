cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasosimulator)

# Administration module
file(GLOB SIMULATOR_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SIMULATOR_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/common)
add_dependencies(${PROJECT_NAME} pasocommon)
add_dependencies(${PROJECT_NAME} translations)
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} pasocommon)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
