cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasoadmin)

# Administration module
file(GLOB ADMIN_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM ADMIN_SOURCES "${PROJECT_SOURCE_DIR}/main.cpp")

add_library(${PROJECT_NAME}lib STATIC ${ADMIN_SOURCES})
target_include_directories(${PROJECT_NAME}lib PUBLIC ${CMAKE_SOURCE_DIR}/common)
target_include_directories(${PROJECT_NAME}lib PUBLIC ${CMAKE_SOURCE_DIR}/ui)
add_dependencies(${PROJECT_NAME}lib pasocommon)
add_dependencies(${PROJECT_NAME}lib pasoui)
add_dependencies(${PROJECT_NAME}lib translations)
target_link_libraries(${PROJECT_NAME}lib Qt5::Core)
target_link_libraries(${PROJECT_NAME}lib Qt5::Widgets)
target_link_libraries(${PROJECT_NAME}lib pasocommon)
target_link_libraries(${PROJECT_NAME}lib pasoui)
enable_coverage(${PROJECT_NAME}lib pasocommon pasoui)

add_executable(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/main.cpp")
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}lib)
