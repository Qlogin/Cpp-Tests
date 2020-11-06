#include "SudokuWidget.h"
#include "CellEdit.h"
#include "CellPopup.h"

#include <QGridLayout>
#include <QFrame>
#include <QKeyEvent>

#include <vector>


/////////////////////////////////////////////////////////////////////

struct SudokuWidget::SudokuWidgetPrivate
{
   SudokuWidgetPrivate(uint N, QWidget * self)
      : max_num(N * N)
   {
      values.resize(max_num * max_num, 0);
      cells.resize(values.size(), nullptr);

      palette[Common]   = self->palette().color(QPalette::Base);
      palette[Active]   = QColor(220, 220, 255);
      palette[Readonly] = QColor(200, 200, 200);
      palette[Invalid]  = QColor(255, 200, 200);
   }

   uint max_num;
   std::vector<uint> values;
   std::vector<CellEdit *> cells;
   CellEdit::Palette palette;
};

/////////////////////////////////////////////////////////////////////

SudokuWidget::SudokuWidget(QWidget *parent, uint N)
   : QWidget(parent)
   , pimpl_(new SudokuWidgetPrivate(N, this))
{
   auto & d = *pimpl_;

   auto layout = new QGridLayout(this);
   layout->setSpacing(1);

   for (uint i = 0; i != d.max_num; ++i)
   {
      auto frame = new QFrame(this);
      frame->setFrameShape(QFrame::Box);

      auto l = new QGridLayout(frame);
      l->setMargin(1);
      l->setSpacing(1);

      for (uint j = 0; j != d.max_num; ++j)
      {
         auto edit = new CellEdit(d.max_num, d.palette, frame);
         l->addWidget(edit, j / N, j % N);

         uint const id = ((i / N) * N + j / N) * d.max_num
                       + ((i % N) * N + j % N);
         d.cells[id] = edit;

         connect(edit, &QLineEdit::textChanged, this,
            [this, id] (QString const & value)
            {
               auto & d = *pimpl_;

               auto new_val = value.toUInt();
               if (new_val != d.values[id])
               {
                  d.values[id] = new_val;
                  emit valueChanged(id / d.max_num, id % d.max_num, new_val);
               }
            });

         edit->setContextMenuPolicy(Qt::CustomContextMenu);
         connect(edit, &QWidget::customContextMenuRequested, this,
            [this, id, N] (QPoint const & pos)
            {
               auto & d = *pimpl_;

               if (!d.cells[id]->isReadOnly())
               {
                  CellPopup popup(this, N);
                  popup.setValue(d.values[id]);
                  popup.move(d.cells[id]->mapToGlobal(pos));

                  if (QDialog::Accepted == popup.exec())
                     setValue(id / d.max_num, id % d.max_num, popup.value());
               }
            });
      }
      layout->addWidget(frame, i / N, i % N);
   }
}

SudokuWidget::~SudokuWidget()
{
}

void SudokuWidget::keyPressEvent( QKeyEvent * e )
{
   auto & d = *pimpl_;

   uint const N = d.max_num * d.max_num;
   uint i = N;
   while (i > 0)
   {
      --i;
      if (d.cells[i]->hasFocus())
         break;
   }

   do
   {
      switch (e->key())
      {
      case Qt::Key_Left:   i -= 1; break;
      case Qt::Key_Right:  i += 1; break;
      case Qt::Key_Up:     i -= d.max_num; break;
      case Qt::Key_Down:   i += d.max_num; break;
      default:
         {
            QWidget::keyPressEvent(e);
            return;
         }
      }
   } while (i < N && d.cells[i]->isReadOnly());

   if (i < N)
      d.cells[i]->setFocus();
   e->accept();
}

void SudokuWidget::setColor(CellType type, QColor const & color)
{
   auto & d = *pimpl_;

   d.palette[type] = color;
   for (auto e : d.cells)
      e->update();
}

QColor SudokuWidget::color(CellType type) const
{
   return pimpl_->palette[type];
}

uint SudokuWidget::value(uint row, uint col) const
{
   auto & d = *pimpl_;

   if (row < d.max_num && col < d.max_num)
      return d.values[row * d.max_num + col];

   return 0;
}

void SudokuWidget::setValue(uint row, uint col, uint value, bool readonly)
{
   auto & d = *pimpl_;

   if (row < d.max_num && col < d.max_num)
   {
      if (value > d.max_num)
         value = 0;

      auto const idx = row * d.max_num + col;
      auto edit = d.cells[idx];
      QSignalBlocker block(edit);

      if (value)
      {
         edit->setText(QString::number(value));
         edit->setReadOnly(readonly);
      }
      else
      {
         edit->setReadOnly(false);
         edit->clear();
      }

      if (d.values[idx] != value)
      {
         d.values[idx] = value;
         emit valueChanged(row, col, value);
      }
   }
}

void SudokuWidget::setReadOnly(uint row, uint col, bool readonly)
{
   auto & d = *pimpl_;

   if (row < d.max_num || col < d.max_num)
   {
      auto const idx = row * d.max_num + col;
      if (d.values[idx])
         d.cells[idx]->setReadOnly(readonly);
   }
}

void SudokuWidget::clear(bool reset_readonly)
{
   auto & d = *pimpl_;

   for (auto e : d.cells)
      if (reset_readonly || !e->isReadOnly())
      {
         e->setReadOnly(false);
         e->clear();
      }

   std::fill(d.values.begin(), d.values.end(), 0);
}
