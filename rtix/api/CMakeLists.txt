# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

# ------------------------------------------------------------------------------
# Run the protobuf compilers
# ------------------------------------------------------------------------------

set(PROTO_TYPES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROTO_CPP_TARGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROTO_PYTHON_TARGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB PROTO_FILES RELATIVE "${PROTO_TYPES_SOURCE_DIR}"
  "*.proto"
)

foreach(PROTO_FILE IN LISTS PROTO_FILES)
  message(STATUS "Generating code for ${PROTO_FILE}")

  # Python - pb2 only
  execute_process(COMMAND python3 -m grpc_tools.protoc
    -I${PROTO_TYPES_SOURCE_DIR} ${PROTO_TYPES_SOURCE_DIR}/${PROTO_FILE}
    --python_out=${PROTO_PYTHON_TARGET_DIR}
  )

  # C++ - pb2 only
  execute_process(COMMAND protoc
    -I${PROTO_TYPES_SOURCE_DIR} ${PROTO_TYPES_SOURCE_DIR}/${PROTO_FILE}
    --cpp_out=${PROTO_CPP_TARGET_DIR}
  )
endforeach()

# ------------------------------------------------------------------------------
# Library target
# ------------------------------------------------------------------------------

set(target_name "rtix-api")

include_directories(
  "${PROJECT_SOURCE_DIR}"
  "${PROTOBUF_INCLUDE_DIR}"
)

file(GLOB PROTO_FILES_PB_CC "${CMAKE_CURRENT_SOURCE_DIR}/*.pb.cc")

add_library(${target_name} SHARED ${PROTO_FILES_PB_CC})

target_link_libraries(${target_name} PRIVATE
  ${PROTOBUF_LIBRARY}
)

set_target_properties(${target_name} PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  POSITION_INDEPENDENT_CODE ON
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
  ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
