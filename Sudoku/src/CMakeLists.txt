set(TARGET_NAME Sudoku)

set(cpps
   main.cpp
   sudoku.cpp
)

file(GLOB header "*.h")

add_executable(${TARGET_NAME} ${cpps} ${headers})

target_link_libraries(${TARGET_NAME} PRIVATE SudokuWidget Qt5::Core)
