cmake_minimum_required(VERSION 3.0)
project(Labyrinth)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to build shared or static library
option(BUILD_SHARED_LIBS "Build shared library" OFF)

find_package(Boost 1.58 REQUIRED
   COMPONENTS
      system
      filesystem
      program_options
)

add_subdirectory(knossos)
add_subdirectory(ariadne)

# Activate testing
include(CTest)

if (BUILD_TESTING)
   enable_testing()
   add_subdirectory(tests)
endif()
