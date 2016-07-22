cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(paso)

# Include our cmake macros and functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Utils)

# Find include files in corresponding build directories (Qt requires this)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc when needed
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run uic when needed
set(CMAKE_AUTOUIC ON)

# Tell compiler to use c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Check for the dependencies
find_package(Qt5Core    5.5 REQUIRED)
find_package(Qt5Network 5.5 REQUIRED)
find_package(Qt5Sql     5.5 REQUIRED)
find_package(Qt5Test    5.5 REQUIRED)
find_package(Qt5Widgets 5.5 REQUIRED)
find_package(Qt5LinguistTools)

# Adding translations
file(GLOB COMMON_HEADERS ${CMAKE_SOURCE_DIR}/common/*.h)
file(GLOB SERVER_HEADERS ${CMAKE_SOURCE_DIR}/server/*.h)
file(GLOB UI_HEADERS ${CMAKE_SOURCE_DIR}/ui/*.h)
file(GLOB ADMIN_HEADERS ${CMAKE_SOURCE_DIR}/admin/*.h)

file(GLOB COMMON_SOURCES ${CMAKE_SOURCE_DIR}/common/*.cpp)
file(GLOB SERVER_SOURCES ${CMAKE_SOURCE_DIR}/server/*.cpp)
file(GLOB UI_SOURCES ${CMAKE_SOURCE_DIR}/ui/*.cpp)
file(GLOB ADMIN_SOURCES ${CMAKE_SOURCE_DIR}/admin/*.cpp)

file(GLOB ADMIN_UIS ${CMAKE_SOURCE_DIR}/admin/*.ui)
file(GLOB UI_UIS ${CMAKE_SOURCE_DIR}/ui/*.ui)

list(APPEND FILES_TO_TRANSLATE ${COMMON_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${SERVER_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${UI_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${ADMIN_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${COMMON_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${SERVER_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${UI_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${ADMIN_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${ADMIN_UIS})
list(APPEND FILES_TO_TRANSLATE ${UI_UIS})

set(TS_FILES translations/${PROJECT_NAME}_en_US.ts)
list(APPEND TS_FILES translations/${PROJECT_NAME}_sr_RS.ts)

option(UPDATE_TRANSLATIONS "Update source translation (files translations/*.ts)." ON)

if(UPDATE_TRANSLATIONS)
    message(STATUS "UPDATE_TRANSLATIONS option is set.")
    qt5_create_translation(QM_FILES ${FILES_TO_TRANSLATE} ${TS_FILES})
else()
    qt5_add_translation(QM_FILES ${TS_FILES})
endif()


add_custom_target(translations DEPENDS ${QM_FILES})
set_property(DIRECTORY PROPERTY CLEAN_NO_CUSTOM TRUE)

# Add subdirectories
add_subdirectory(admin)
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(ui)
add_subdirectory(tests)

