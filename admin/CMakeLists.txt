cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(pasoadmin)

message(STATUS "INCLUDE DIRS: ${PASOCOMMON_INCLUDE_DIRS}")
message(STATUS "LIBRARIES   : ${PASOCOMMON_LIBRARIES}")
# Administration module
file(GLOB ADMIN_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(pasoadmin ${ADMIN_SOURCES})
target_include_directories(pasoadmin PUBLIC ${PASOCOMMON_INCLUDE_DIRS})
target_link_libraries(pasoadmin Qt5::Core)
target_link_libraries(pasoadmin Qt5::Widgets)
target_link_libraries(pasoadmin ${PASOCOMMON_LIBRARIES})
add_dependencies(pasoadmin pasocommon)
