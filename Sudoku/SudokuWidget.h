#ifndef SUDOKUWIDGET_H
#define SUDOKUWIDGET_H

#include <QWidget>
#include <QVector>

class QLineEdit;

class SudokuWidget : public QWidget
{
   Q_OBJECT

public:
   SudokuWidget(QWidget *parent = 0, uint N = 3);
   ~SudokuWidget();

public slots:
   void clear();

private:
   void keyPressEvent( QKeyEvent * e ) override;

private:
   uint max_num_;
   QVector<QLineEdit *> edits_;
};

#endif // SUDOKUWIDGET_H
