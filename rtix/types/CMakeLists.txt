# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

set(module_name "types")

# ------------------------------------------------------------------------------
# Run the protobuf compilers
# ------------------------------------------------------------------------------

set(PROTO_PREFIX "rtix/types")

file(GLOB PROTO_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "*.proto"
)

foreach(PROTO_FILE IN LISTS PROTO_FILES)
  message(STATUS "Generating code for ${PROTO_PREFIX}/${PROTO_FILE}")

  # C++
  execute_process(COMMAND protoc
    ${PROTO_PREFIX}/${PROTO_FILE}
    -I=${PROJECT_SOURCE_DIR}
    --cpp_out=${PROJECT_SOURCE_DIR}
  )

  # Python
  execute_process(COMMAND python3 -m grpc_tools.protoc
    ${PROTO_PREFIX}/${PROTO_FILE}
    -I=${PROJECT_SOURCE_DIR}
    --python_out=${PROJECT_SOURCE_DIR}
  )

endforeach()

# ------------------------------------------------------------------------------
# Library target
# ------------------------------------------------------------------------------

set(target_name "rtix-${module_name}")

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROTOBUF_INCLUDE_DIR}
)

file(GLOB PROTO_FILES_PB_CC "${CMAKE_CURRENT_SOURCE_DIR}/*.pb.cc")

add_library(${target_name} SHARED ${PROTO_FILES_PB_CC})

target_link_libraries(${target_name} PRIVATE
  ${PROTOBUF_LIBRARY}
)

file(GLOB_RECURSE MODULE_HEADERS "*.h")

set_target_properties(${target_name} PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  POSITION_INDEPENDENT_CODE ON
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
  ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
  PUBLIC_HEADER "${MODULE_HEADERS}"
)

install(TARGETS ${target_name}
  EXPORT rtixTargets
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/rtix/${module_name}"
)

install(
  FILES "${PROTO_FILES}"
  DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/rtix/${module_name}"
)
