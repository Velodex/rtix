# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(rtix-cmake-import)

find_package(rtix CONFIG REQUIRED)

include_directories(${RTIX_INCLUDE_DIRS})

add_executable(rtix-cmake-import main.cpp)

target_link_libraries(rtix-cmake-import ${RTIX_LIBRARIES})
