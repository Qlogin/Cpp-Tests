cmake_minimum_required(VERSION 3.0)
project(LongArithm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Option to build shared or static library
option(BUILD_SHARED_LIBS "Build shared library" OFF)

set(LIBRARY_NAME ${PROJECT_NAME})

# Type is specified by BUILD_SHARED_LIBS option
add_library(${LIBRARY_NAME}
  long_number.h
  long_number.cpp
)

# Generate export header
#   (realy needed only for shared library)
include(GenerateExportHeader)
generate_export_header(${LIBRARY_NAME}
  EXPORT_FILE_NAME ${LIBRARY_NAME}/export.h
)

# Paths to public headers and generated export header
target_include_directories(${LIBRARY_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

set(TEST_NAME ${LIBRARY_NAME}Test)

add_executable(${TEST_NAME} test.cpp)
target_link_libraries(${TEST_NAME} PRIVATE ${LIBRARY_NAME})