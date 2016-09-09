cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(paso)

option(CMAKE_ENABLE_COVERAGE "Controls whether to enable code coverage on Debug build." OFF)

# Include our cmake macros and functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Utils)

# Find include files in corresponding build directories (Qt requires this)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc when needed
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run uic when needed
set(CMAKE_AUTOUIC ON)
# Instruct CMake to run rcc when needed
set(CMAKE_AUTORCC ON)


# Tell compiler to use c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Check for the dependencies
find_package(Qt5Core    5.5 REQUIRED)
find_package(Qt5Network 5.5 REQUIRED)
find_package(Qt5Sql     5.5 REQUIRED)
find_package(Qt5Test    5.5 REQUIRED)
find_package(Qt5Widgets 5.5 REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_program(OPENSSL openssl DOC "Looking for openssl executable.")

if(NOT OPENSSL)
    message(FATAL_ERROR "The OpenSSL executable was not found.")
endif(NOT OPENSSL)

# Adding translations
file(GLOB COMMON_HEADERS ${CMAKE_SOURCE_DIR}/common/*.h)
file(GLOB SERVER_HEADERS ${CMAKE_SOURCE_DIR}/server/*.h)
file(GLOB UI_HEADERS ${CMAKE_SOURCE_DIR}/ui/*.h)
file(GLOB ADMIN_HEADERS ${CMAKE_SOURCE_DIR}/admin/*.h)
file(GLOB SIMULATOR_HEADERS ${CMAKE_SOURCE_DIR}/simulator/*.h)

file(GLOB COMMON_SOURCES ${CMAKE_SOURCE_DIR}/common/*.cpp)
file(GLOB SERVER_SOURCES ${CMAKE_SOURCE_DIR}/server/*.cpp)
file(GLOB UI_SOURCES ${CMAKE_SOURCE_DIR}/ui/*.cpp)
file(GLOB ADMIN_SOURCES ${CMAKE_SOURCE_DIR}/admin/*.cpp)
file(GLOB SIMULATOR_SOURCES ${CMAKE_SOURCE_DIR}/simulator/*.cpp)

file(GLOB ADMIN_UIS ${CMAKE_SOURCE_DIR}/admin/*.ui)
file(GLOB UI_UIS ${CMAKE_SOURCE_DIR}/ui/*.ui)
file(GLOB SIMULATOR_UIS ${CMAKE_SOURCE_DIR}/simulator/*.ui)

list(APPEND FILES_TO_TRANSLATE ${COMMON_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${SERVER_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${UI_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${ADMIN_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${SIMULATOR_HEADERS})
list(APPEND FILES_TO_TRANSLATE ${COMMON_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${SERVER_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${UI_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${ADMIN_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${SIMULATOR_SOURCES})
list(APPEND FILES_TO_TRANSLATE ${ADMIN_UIS})
list(APPEND FILES_TO_TRANSLATE ${UI_UIS})
list(APPEND FILES_TO_TRANSLATE ${SIMULATOR_UIS})

set(TS_FILES translations/${PROJECT_NAME}_sr_RS.ts)

option(UPDATE_TRANSLATIONS "Update source translation (files translations/*.ts)." ON)

if(UPDATE_TRANSLATIONS)
    message(STATUS "UPDATE_TRANSLATIONS option is set.")
    qt5_create_translation(QM_FILES ${FILES_TO_TRANSLATE} ${TS_FILES})
else()
    qt5_add_translation(QM_FILES ${TS_FILES})
endif()

add_custom_target(translations DEPENDS ${QM_FILES})
set_property(DIRECTORY PROPERTY CLEAN_NO_CUSTOM TRUE)

file(STRINGS ${CMAKE_SOURCE_DIR}/VERSION COMPONENT_VERSION)

add_definitions(-DVERSION="${COMPONENT_VERSION}")

configure_file(${CMAKE_SOURCE_DIR}/translations/translations.qrc ${CMAKE_BINARY_DIR}/translations.qrc @ONLY)

# Add subdirectories
add_subdirectory(admin)
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(ui)
add_subdirectory(simulator)
add_subdirectory(tests)

configure_file(${CMAKE_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_BINARY_DIR}/doc/Doxyfile @ONLY)

add_custom_target(doc COMMAND doxygen WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc)
