# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

set(module_name "process")

# ------------------------------------------------------------------------------
# Example target
# ------------------------------------------------------------------------------
set(target_name "rtix-${module_name}")

# Header only
add_library(${target_name} INTERFACE)

file(GLOB_RECURSE MODULE_HEADERS "*.h")

set_target_properties(${target_name} PROPERTIES
  PUBLIC_HEADER "${MODULE_HEADERS}"
)

install(TARGETS ${target_name}
  EXPORT rtixTargets
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/rtix/${module_name}"
)

# ------------------------------------------------------------------------------
# Test target
# ------------------------------------------------------------------------------
set(test_name "rtix-${module_name}-test")

add_executable(${test_name} 
  tests/test_process.cpp
)

add_dependencies(${test_name} 
  rtix-api
  rtix-ipc
)

target_link_libraries(${test_name} PRIVATE
  ${target_name}
  ${GTEST_LIBRARIES}
  ${GTEST_MAIN_LIBRARIES}
  rtix-api
  rtix-ipc
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