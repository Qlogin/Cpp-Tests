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

   uint value(uint row, uint col) const;

public slots:
   void setValue(uint row, uint col, uint value);
   void clear();

private:
   void keyPressEvent( QKeyEvent * e ) override;

private:
   uint max_num_;
   QVector<QLineEdit *> edits_;
};

#endif // SUDOKUWIDGET_H
