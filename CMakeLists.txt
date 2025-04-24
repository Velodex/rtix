# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

# ------------------------------------------------------------------------------
# Settings
# ------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0054 NEW)

# Get the project version info
file(READ "version.txt" ver)
string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${ver})
set(VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${ver})
set(VERSION_MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_PATCH ([0-9]*)" _ ${ver})
set(VERSION_PATCH ${CMAKE_MATCH_1})

# Set the project info
set(PROJ_NAME rtix)
set(PROJ_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

# Create the project
project(${PROJ_NAME} 
  LANGUAGES C CXX
  VERSION ${PROJ_VERSION}
)

# CTest
enable_testing()
include(CTest)

# Add compiler flags
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Set build type
if(TEST_COVERAGE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")

# ------------------------------------------------------------------------------
# CMake modules
# ------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(CMakePackageConfigHelpers)  # To build cmake configs
include(GNUInstallDirs)  # For find_package(rtix) to work

if(TEST_COVERAGE)
  include(CodeCoverage)
  APPEND_COVERAGE_COMPILER_FLAGS()
endif()

# ------------------------------------------------------------------------------
# Find depdendencies
# ------------------------------------------------------------------------------

include(FindProtobuf)
find_package(nng CONFIG REQUIRED)
find_package(yaml-cpp 0.6.2 REQUIRED)
find_package(spdlog 1.5.0 CONFIG REQUIRED)
find_package(cpptrace 0.8.3 REQUIRED)
find_package(Protobuf 3.6.1 REQUIRED)
find_package(GTest REQUIRED)

# Dependency include directories
include_directories(${PROJECT_SOURCE_DIR})

# ------------------------------------------------------------------------------
# This is needed to be able to display compile-time debug logs
# ------------------------------------------------------------------------------
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

# ------------------------------------------------------------------------------
# Add the library targets
# ------------------------------------------------------------------------------
add_subdirectory(rtix)

# ------------------------------------------------------------------------------
# Setup the coverage reporting
# ------------------------------------------------------------------------------
if(TEST_COVERAGE)
  set(COVERAGE_GCOVR_EXCLUDES
    ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_SOURCE_DIR}/examples
  )

  set(COVERAGE_DEPENDENCIES
    rtix-core
    rtix-ipc
  )

  SETUP_TARGET_FOR_COVERAGE_GCOVR_XML(
    NAME coverage-xml
    EXECUTABLE ctest
    DEPENDENCIES ${COVERAGE_DEPENDENCIES}
  )

  SETUP_TARGET_FOR_COVERAGE_GCOVR_HTML(
    NAME coverage-html
    EXECUTABLE ctest
    DEPENDENCIES ${COVERAGE_DEPENDENCIES}
  )
endif()
