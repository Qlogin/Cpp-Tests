
QT += core gui widgets

TARGET = SudokuWidget
TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES += src/SudokuWidget.cpp \
   src/CellEdit.cpp

HEADERS += include/SudokuWidget.h \
   src/CellEdit.h
INCLUDEPATH += include/
