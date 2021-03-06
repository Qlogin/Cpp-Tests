#pragma once

#include <QWidget>
#include <memory>


class SudokuWidget : public QWidget
{
   Q_OBJECT

public:
   SudokuWidget(QWidget * parent = nullptr, uint N = 3);
   ~SudokuWidget();

   uint value(uint row, uint col) const;

   enum CellType
   {
      Common,
      Active,
      Readonly,
      Invalid,
   };

   void   setColor(CellType type, QColor const & color);
   QColor color(CellType type) const;

public slots:
   void setValue(uint row, uint col, uint value, bool readonly = false);
   void setReadOnly(uint row, uint col, bool readonly);
   void clear(bool reset_readonly = false);

signals:
   void valueChanged(uint row, uint col, uint value);

private:
   void keyPressEvent( QKeyEvent * e ) override;

private:
   struct SudokuWidgetPrivate;
   std::unique_ptr<SudokuWidgetPrivate> pimpl_;
};
