set(TARGET_NAME SudokuWidget)

set(cpps
   src/CellEdit.cpp
   src/CellPopup.cpp
   src/SudokuWidget.cpp
)

set(headers
   src/CellEdit.h
   src/CellPopup.h
   include/SudokuWidget.h
)

add_library(${TARGET_NAME} STATIC ${cpps} ${headers})

target_include_directories(${TARGET_NAME} PUBLIC include)
target_link_libraries(${TARGET_NAME} PUBLIC Qt5::Widgets PRIVATE Qt5::Core)
