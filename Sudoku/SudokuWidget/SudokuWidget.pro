
QT += core gui widgets

TARGET = SudokuWidget
TEMPLATE = lib
CONFIG += staticlib c++14

SOURCES += src/SudokuWidget.cpp \
   src/CellEdit.cpp \
   src/CellPopup.cpp

HEADERS += include/SudokuWidget.h \
   src/CellEdit.h \
   src/CellPopup.h
INCLUDEPATH += include/
