cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasoadmin)

# Administration module
file(GLOB ADMIN_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(pasoadmin ${ADMIN_SOURCES})
target_include_directories(pasoadmin PUBLIC ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(pasoadmin Qt5::Core)
target_link_libraries(pasoadmin Qt5::Widgets)
target_link_libraries(pasoadmin pasocommon)
add_dependencies(pasoadmin pasocommon)
