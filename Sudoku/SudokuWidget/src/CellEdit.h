#pragma once

#include <QLineEdit>
#include <QColor>

#include <array>


class CellEdit : public QLineEdit
{
public:
   typedef std::array<QColor, 4> Palette;

   CellEdit(uint max_num, Palette const & palette,
            QWidget * parent = nullptr);

   void setReadOnly( bool readonly );
   void update();

private:
   void setBackground( QColor const & color );

private:
   QSize sizeHint() const override;

   void keyPressEvent(QKeyEvent * e) override;
   void focusInEvent(QFocusEvent * e) override;
   void focusOutEvent(QFocusEvent * e) override;

private:
   Palette const & palette_;
};
