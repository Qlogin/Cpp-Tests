#include "sudoku.h"
#include "SudokuWidget.h"

#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>


struct MainWindow : QMainWindow
{
   MainWindow()
      : sudoku_(new SudokuWidget(this, 3))
   {
      setCentralWidget(sudoku_);

      auto mb = menuBar();
      auto file = mb->addMenu(tr("&File"));

      connect(file->addAction(tr("&New")), &QAction::triggered,
              [this] { sudoku_->clear(true); });
      connect(file->addAction(tr("&Open")),
              &QAction::triggered, this, &MainWindow::open);
      connect(file->addAction(tr("&Save")),
              &QAction::triggered, this, &MainWindow::save);
      file->addSeparator();
      connect(file->addAction(tr("E&xit")),
              &QAction::triggered, this, &QMainWindow::close);

      auto puzzle = mb->addMenu(tr("Puzzle"));

      connect(puzzle->addAction(tr("Check")),
              &QAction::triggered, this, &MainWindow::check);
      connect(puzzle->addAction(tr("Solve")),
              &QAction::triggered, this, &MainWindow::solve);
      connect(puzzle->addAction(tr("Clear")),
              &QAction::triggered, sudoku_, &SudokuWidget::clear);

      settings_.beginGroup("MainWindow");
      restoreGeometry(settings_.value("geometry").toByteArray());
   }

   ~MainWindow()
   {
      settings_.setValue("geometry", saveGeometry());
   }

private:
   void open()
   {
      auto filename = settings_.value("recent-file").toString();

      filename = QFileDialog::getOpenFileName(this, tr("Open Sudoku"), filename,
                                              tr("Simple Sudoku Format (*.sud)"));
      if (filename.isEmpty())
         return;

      sudoku::fields_t fields {};
      if (!sudoku::load(filename, fields))
      {
         QMessageBox::critical(this, tr("Error"), tr("Failed to load Sudoku!"));
         return;
      }

      sudoku_->clear(true);
      setValues(fields, true);

      settings_.setValue("recent-file", filename);
   }

   void save()
   {
      auto filename = settings_.value("recent-file").toString();

      filename = QFileDialog::getSaveFileName(this, tr("Open Sudoku"), filename,
                                              tr("Simple Sudoku Format (*.sud)"));
      if (filename.isEmpty())
         return;

      if (!sudoku::save(filename, values()))
      {
         QMessageBox::critical(this, tr("Error"), tr("Failed to save Sudoku!"));
         return;
      }

      settings_.setValue("recent-file", filename);
   }

   void check()
   {
   }

   void solve()
   {
      sudoku::fields_t fields = values();
      if (!sudoku::solve(fields))
      {
         QMessageBox::critical(this, tr("Error"), tr("Failed to solve Sudoku!"));
         return;
      }
      setValues(fields, false);
   }

private:
   void setValues(sudoku::fields_t const & fields, bool readonly)
   {
      for (uint i = 0; i != sudoku::Size; ++i)
         for (uint j = 0; j != sudoku::Size; ++j)
         {
            if (fields[i][j] && !sudoku_->value(i, j))
               sudoku_->setValue(i, j, fields[i][j], readonly);
         }
   }

   sudoku::fields_t values() const
   {
      sudoku::fields_t fields {};
      for (uint i = 0; i != sudoku::Size; ++i)
         for (uint j = 0; j != sudoku::Size; ++j)
            fields[i][j] = sudoku_->value(i, j);
      return fields;
   }

private:
   SudokuWidget * sudoku_;
   QSettings settings_;
};

/////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   QApplication::setApplicationName("Sudoku");
   QApplication::setOrganizationName("Qlogin");

   MainWindow mw;
   mw.show();

   return a.exec();
}
