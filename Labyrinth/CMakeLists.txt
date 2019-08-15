cmake_minimum_required(VERSION 3.0)

project(Labyrinth)

find_package(Boost 1.65 REQUIRED
   COMPONENTS
      system
      filesystem
      program_options
)

# Option to build shared or static library
option(BUILD_SHARED_LIBS "Build shared library" OFF)

add_subdirectory(knossos)
add_subdirectory(ariadne)
