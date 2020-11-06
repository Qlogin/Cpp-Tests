#pragma once

#include <QDialog>
#include <vector>


class QToolButton;

class CellPopup : public QDialog
{
   Q_OBJECT
   Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged USER true)

public:
   CellPopup(QWidget * parent, uint N);

   uint value() const;
   void setValue( uint value );
   void enableValues( std::vector<uint> const & values, bool enable );

signals:
   void valueChanged( uint );

private:
   uint value_ = 0;
   std::vector<QToolButton *> buttons_;
};
