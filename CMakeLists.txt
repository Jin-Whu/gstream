cmake_minimum_required(VERSION 3.13)
project(streamcxx)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)


set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited build type configuration" FORCE)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Set build type to Debug or Release (default Release)" FORCE)
endif()

# windows runtime library configuration
if(MSVC)
    option(MSVC_STATIC_RUNTIME "link static runtime libraries" OFF)
    if(MSVC_STATIC_RUNTIME)
        foreach(flag_var CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        if (${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
        endforeach()
    endif()
endif()

# debug
set(CMAKE_DEBUG_POSTFIX d)

# set position independent
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# include external project
option(ENABLE_STREAM_LOG "enable stream log" OFF)
include(${PROJECT_SOURCE_DIR}/cmake/external.cmake)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)

# option
option(BUILD_EXAMPLE "build example" ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/example)