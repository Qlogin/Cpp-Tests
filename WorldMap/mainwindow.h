#pragma once

#include <QMainWindow>
#include <QScopedPointer>

namespace Ui
{
   class MainWindow;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(QWidget *parent = 0);
   ~MainWindow();

private:
   struct impl_t;
   QScopedPointer<impl_t> pimpl_;
};
