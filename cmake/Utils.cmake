# Check if compiler supports C++11 standard
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
if (NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} version ${CMAKE_CXX_COMPILER_VERSION} has no c++11 support. Please use different C++ compiler.")
endif ()

# Fix RPATH policy on newer OS X versions
if (NOT DEFINED CMAKE_MACOSX_RPATH)
    set(CMAKE_MACOSX_RPATH 0)
endif ()

# Set project wide compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused")

# Find required executables
find_program(GCOV gcov)
find_program(GCOVR gcovr)
find_program(GENHTML genhtml)
find_program(LCOV lcov)
find_program(PYTHON python)
find_program(CPPCHECK cppcheck)
find_program(DOXYGEN doxygen)

# Abort if required executables are not found
if (NOT GCOV)
    message(FATAL_ERROR "The gcov executable not found!")
endif ()
if (NOT PYTHON)
    message(FATAL_ERROR "The python executable not found!")
endif ()
if (NOT LCOV)
    message(FATAL_ERROR "The lcov executable not found!")
endif ()
if (NOT GENHTML)
    message(FATAL_ERROR "The genhtml executable not found!")
endif ()
if (NOT GCOVR)
    message(FATAL_ERROR "The gcovr executable not found!")
endif ()
if (NOT CPPCHECK)
    message(FATAL_ERROR "The cppcheck executable not found!")
endif ()
if (NOT DOXYGEN)
    message(FATAL_ERROR "The doxygen executable not found!")
endif ()

# Detect compiler
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} "GNU" USING_GCC)
string(REGEX MATCH ".*Clang" USING_CLANG ${CMAKE_CXX_COMPILER_ID})

# This function sets target properties suitable for generating code coverage.
#
# @param TARGET the target for which to enable code coverage
# @param IS_TEST denotes whether target is test executable and needs to be added to the coverage target.
function(enable_coverage TARGET IS_TEST)
    if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        message(STATUS "Build type is not Debug. The coverage will not be enabled for target ${TARGET}.")
        return()
    endif()
    # Getting important target properties
    get_target_property(TARGET_SOURCES ${TARGET} SOURCES)
    get_target_property(TARGET_INCLUDES ${TARGET} INCLUDE_DIRECTORIES)
    get_target_property(TARGET_COMPILE_DEFINITIONS ${TARGET} COMPILE_DEFINITIONS)
    get_target_property(TARGET_COMPILE_FLAGS ${TARGET} COMPILE_FLAGS)
    get_target_property(TARGET_LINK_LIBRARIES ${TARGET} LINK_LIBRARIES)
    get_target_property(TARGET_TYPE ${TARGET} TYPE)

    # This is a fix for CMake version < 3.0 weird handling of the properties that are not found.
    if ("${TARGET_INCLUDES}" STREQUAL "TARGET_INCLUDES-NOTFOUND")
        set(TARGET_INCLUDES "")
    endif ()
    if ("${TARGET_COMPILE_DEFINITIONS}" STREQUAL "TARGET_COMPILE_DEFINITIONS-NOTFOUND")
        set(TARGET_COMPILE_DEFINITIONS "")
    endif ()
    if ("${TARGET_COMPILE_FLAGS}" STREQUAL "TARGET_COMPILE_FLAGS-NOTFOUND")
        set(TARGET_COMPILE_FLAGS "")
    endif ()
    if ("${TARGET_LINK_LIBRARIES}" STREQUAL "TARGET_LINK_LIBRARIES-NOTFOUND")
        set(TARGET_LINK_LIBRARIES "")
    endif ()

    # Setting coverage compiler and linker flags
    if (USING_GCC)
        list(APPEND TARGET_LINK_LIBRARIES gcov)
    endif ()

    list(APPEND COVERAGE_FLAGS --coverage)
    list(APPEND COVERAGE_FLAGS -O0)

    # Create a string version of coverage flags
    string(REPLACE ";" " " COVERAGE_FLAGS_STR "${COVERAGE_FLAGS}")

    # Adding coverage enabled target
    message(STATUS "Enabling code coverage for target '${TARGET}'")

    # Adding compile flags
    if (TARGET_COMPILE_FLAGS)
        set(TARGET_COMPILE_FLAGS "${TARGET_COMPILE_OPTIONS} ${COVERAGE_FLAGS_STR}")
    else ()
        set(TARGET_COMPILE_FLAGS "${COVERAGE_FLAGS_STR}")
    endif ()
    set_property(TARGET ${TARGET} PROPERTY COMPILE_FLAGS ${TARGET_COMPILE_FLAGS})

    # Add libraries
    if (TARGET_LINK_LIBRARIES)
        target_link_libraries(${TARGET} ${TARGET_LINK_LIBRARIES})
    endif ()
    if (USING_CLANG)
        get_target_property(LINK_FLAGS ${TARGET} TARGET_LINK_FLAGS)
        set(COVERAGE_LINK_FLAGS "${TARGET_LINK_FLAGS} ${COVERAGE_FLAGS_STR}")
        set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${COVERAGE_LINK_FLAGS}")
    endif ()
    message(STATUS "${TARGET} link libraries: ${TARGET_LINK_LIBRARIES}")

    if (TARGET_TYPE STREQUAL EXECUTABLE AND IS_TEST STREQUAL "TRUE")
        # Adding coverage target
        add_custom_target(coverage-${TARGET}
            COMMAND ${TARGET}
            COMMAND mkdir -p ${PROJECT_BINARY_DIR}/coverage
            COMMAND ${LCOV} --capture
            --no-external
            --rc lcov_branch_coverage=1
            --base-directory ${CMAKE_SOURCE_DIR}
            --directory=${CMAKE_BINARY_DIR}
            --output-file=${PROJECT_BINARY_DIR}/coverage/coverage-${TARGET}.all.info
            COMMAND ${LCOV}
            --rc lcov_branch_coverage=1
            --remove ${PROJECT_BINARY_DIR}/coverage/coverage-${TARGET}.all.info
            'test*'
            --output-file=${PROJECT_BINARY_DIR}/coverage/coverage-${TARGET}.without-tests.info
            COMMAND ${LCOV}
            --rc lcov_branch_coverage=1
            --remove ${PROJECT_BINARY_DIR}/coverage/coverage-${TARGET}.without-tests.info
            'build*'
            --output-file=${PROJECT_BINARY_DIR}/coverage/coverage-${TARGET}.final.info
            COMMAND ${LCOV}
            --rc lcov_branch_coverage=1
            --summary=${PROJECT_BINARY_DIR}/coverage/coverage-${TARGET}.final.info 2> ${PROJECT_BINARY_DIR}/coverage/coverage-${TARGET}.final.summary
            COMMAND ${GENHTML} ${PROJECT_BINARY_DIR}/coverage/coverage-${TARGET}.final.info
            --title ${PROJECT_NAME}
            --legend
            --show-details
            --function-coverage
            --branch-coverage
            --demangle-cpp
            --output-directory ${PROJECT_BINARY_DIR}/coverage/${TARGET}
            COMMAND cd ${PROJECT_BINARY_DIR} && ${GCOVR}
            --filter=.*/src/.*
            --exclude=.*/build/.*
            --xml-pretty > ${PROJECT_BINARY_DIR}/coverage-${TARGET}.final.xml
            DEPENDS ${TARGET})
        if(NOT TARGET coverage)
            add_custom_target(coverage
                COMMAND mkdir -p ${CMAKE_BINARY_DIR}/coverage
                COMMAND find . -name *final.info -exec sh -c "echo -a {}" \\; \| xargs lcov -o ${CMAKE_BINARY_DIR}/combined-coverage.info
                COMMAND ${GENHTML} ${CMAKE_BINARY_DIR}/combined-coverage.info
                --title "Combined coverage report for ${CMAKE_PROJECT_NAME}"
                --legend
                --show-details
                --function-coverage
                --demangle-cpp
                --output-directory ${CMAKE_BINARY_DIR}/coverage
                COMMAND cd ${CMAKE_BINARY_DIR} && ${GCOVR}
                --filter=.*/src/.*
                --exclude=.*/build/.*
                --xml-pretty > ${CMAKE_BINARY_DIR}/coverage.xml
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                VERBATIM)
        endif()
        add_dependencies(coverage coverage-${TARGET})
    endif ()
    if (NOT TARGET clean-coverage)
        add_custom_target(clean-coverage COMMAND lcov --directory ${CMAKE_BINARY_DIR} --zerocounters)
    endif()
endfunction()

# This function creates new target that performs static code analysis. It is enough to
# enable cppcheck only once, as it will check whole source tree. Multiple enablements
# will be ignored.
#
# @param TARGET_TO_CHECK The target before which cppcheck should be performed. Usually main binary, or unittests.
function(enable_cppcheck TARGET_TO_CHECK)
    if (TARGET cppcheck)
        message(WARNING "The static code analysis is already enabled.")
    else ()
        message(STATUS "Enabling cppcheck static code analysis for target '${TARGET_TO_CHECK}'")
        set(INCLUDE_SECTION "")
        get_target_property(TARGET_INCLUDE_DIRS ${TARGET_TO_CHECK} INCLUDE_DIRECTORIES)
        if (NOT "${TARGET_INCLUDE_DIRS}" STREQUAL "TARGET_INCLUDE_DIRS-NOTFOUND")
            foreach(_DIR "${TARGET_INCLUDE_DIRS}")
                if ("${_DIR}" MATCHES "^${CMAKE_SOURCE_DIR}.*")
                    string(CONCAT INCLUDE_SECTION "${INCLUDE_SECTION} -I ${_DIR}")
                endif()
            endforeach()
            separate_arguments(INCLUDE_SECTION)
        endif()
        get_target_property(TARGET_SOURCES ${TARGET_TO_CHECK} SOURCES)
        separate_arguments(TARGET_SOURCES)
        add_custom_target(cppcheck
            COMMAND ${CPPCHECK}
            --xml
            --xml-version=2
            --enable=all
            --suppress=missingInclude
            ${INCLUDE_SECTION}
            ${TARGET_SOURCES} 2> ${PROJECT_BINARY_DIR}/cppcheck.xml
            COMMAND ${CPPCHECK}-htmlreport
            --title=${TARGET_TO_CHECK}
            --file=${PROJECT_BINARY_DIR}/cppcheck.xml
            --report-dir=${PROJECT_BINARY_DIR}/cppcheck
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Performing static code analysis on ${TARGET_TO_CHECK}.")
        if ("${CMAKE_AUTO_CPPCHECK}" MATCHES "ON")
            add_dependencies(${TARGET_TO_CHECK} cppcheck)
        endif ()
    endif ()
endfunction()
