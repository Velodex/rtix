# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

set(module_name "core")

# ------------------------------------------------------------------------------
# Example target
# ------------------------------------------------------------------------------
set(target_name "rtix-${module_name}")

include_directories(
  ${PROJECT_SOURCE_DIR}
)

add_library(${target_name} SHARED
  logging.cpp
  timer.cpp
)

# add_dependencies(${target_name}
#   # No dependencies
# )

# target_link_libraries(${target_name} PRIVATE
#   # Nothing to link
# )

set_target_properties(${target_name} PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  POSITION_INDEPENDENT_CODE ON
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
  ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# ------------------------------------------------------------------------------
# Test target
# ------------------------------------------------------------------------------
set(test_name "rtix-${module_name}-test")

add_executable(${test_name} 
  tests/test_format.cpp
  tests/test_logging.cpp
  tests/test_timer.cpp
)

add_dependencies(${test_name} 
  ${target_name}
)

target_link_libraries(${test_name} PRIVATE
  ${target_name}
  ${GTEST_LIBRARIES}
  ${GTEST_MAIN_LIBRARIES}
  pthread
)

set_target_properties(${test_name} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_test(
  NAME ${test_name}
  COMMAND ${test_name}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# ------------------------------------------------------------------------------
# PyTest target
# ------------------------------------------------------------------------------
set(py_test_name "rtix-${module_name}-test-py")

add_test (NAME ${py_test_name}
  COMMAND pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

