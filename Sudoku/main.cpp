#include "SudokuWidget.h"

#include <QMainWindow>
#include <QApplication>
#include <QToolBar>


int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   QMainWindow mw;

   auto sudoku = new SudokuWidget(&mw, 3);
   mw.setCentralWidget(sudoku);

   auto tb = mw.addToolBar(QString("Edit"));
   tb->addAction(QString("Clear"), sudoku, &SudokuWidget::clear);

   mw.show();
   return a.exec();
}
