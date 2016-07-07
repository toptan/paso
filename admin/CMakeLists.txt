cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasoadmin)

# Administration module
file(GLOB ADMIN_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(${PROJECT_NAME} ${ADMIN_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} pasocommon)
add_dependencies(${PROJECT_NAME} pasocommon)
add_dependencies(${PROJECT_NAME} translations)
