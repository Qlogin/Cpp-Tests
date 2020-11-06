#include "CellPopup.h"

#include <QGridLayout>
#include <QToolButton>
#include <QSignalBlocker>


CellPopup::CellPopup(QWidget * parent, uint N)
   : QDialog(parent, Qt::Popup)
{
   auto layout = new QGridLayout(this);
   layout->setSpacing(1);
   layout->setMargin(5);

   buttons_.reserve(N * N);
   for (uint i = 0; i != N; ++i)
      for (uint j = 0; j != N; ++j)
      {
         auto btn = new QToolButton(this);
         buttons_.push_back(btn);

         btn->setAutoRaise(true);
         btn->setCheckable(true);
         btn->setFixedSize(30, 30);
         btn->setText(QString::number(buttons_.size()));
         layout->addWidget(btn, i, j);

         connect(btn, &QToolButton::toggled, this,
            [this, val = buttons_.size()] (bool checked)
            {
               setValue(checked ? val : 0);
               accept();
            });
      }
}

uint CellPopup::value() const
{
   return value_;
}

void CellPopup::setValue(uint new_value)
{
   if (new_value != value_ && new_value <= buttons_.size())
   {
      if (value_)
      {
         QSignalBlocker block(buttons_[value_ - 1]);
         buttons_[value_ - 1]->setChecked(false);
      }
      if (new_value)
      {
         QSignalBlocker block(buttons_[new_value - 1]);
         buttons_[new_value - 1]->setChecked(true);
      }

      value_ = new_value;
      emit valueChanged(value_);
   }
}
