#include "SudokuWidget.h"

#include <QMainWindow>
#include <QApplication>
#include <QToolBar>


struct MainWindow : QMainWindow
{
   MainWindow()
      : sudoku_(new SudokuWidget(this, 3))
   {
      setCentralWidget(sudoku_);

      auto tb = addToolBar(QString("Edit"));

      connect(tb->addAction(QString("Open")),
              &QAction::triggered, this, &MainWindow::open);
      connect(tb->addAction(QString("Solve")),
              &QAction::triggered, this, &MainWindow::solve);
      connect(tb->addAction(QString("Clear")),
              &QAction::triggered, sudoku_, &SudokuWidget::clear);
   }

private:
   void open()
   {
   }

   void solve()
   {
   }

private:
   SudokuWidget * sudoku_;
};

/////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   MainWindow mw;
   mw.show();

   return a.exec();
}
