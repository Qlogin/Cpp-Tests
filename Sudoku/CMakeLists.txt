cmake_minimum_required(VERSION 3.0)
project(Sudoku)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

find_package(Qt5 COMPONENTS Core Gui Widgets)

add_subdirectory(SudokuWidget)
add_subdirectory(src)
