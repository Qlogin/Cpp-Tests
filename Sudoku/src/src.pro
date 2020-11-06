QT += core gui widgets

INCLUDEPATH += $$PWD/../SudokuWidget/include
DEPENDPATH += $$PWD/../SudokuWidget/include

unix: LIBS += -L$$OUT_PWD/../SudokuWidget/ -lSudokuWidget
unix: PRE_TARGETDEPS += $$OUT_PWD/../SudokuWidget/libSudokuWidget.a

TARGET = Sudoku
TEMPLATE = app
CONFIG += c++11
SOURCES += main.cpp \
   sudoku.cpp

HEADERS += \
   sudoku.h
