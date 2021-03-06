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
      , N(N)
   {
      values.resize(max_num * max_num, 0);
      cells.resize(values.size(), nullptr);

      palette[Common]   = self->palette().color(QPalette::Base);
      palette[Active]   = QColor(220, 220, 255);
      palette[Readonly] = QColor(200, 200, 200);
      palette[Invalid]  = QColor(255, 200, 200);
   }

   void validate(uint row, uint col, bool with_neighbours = true)
   {
      bool const was_invalid = cells[row * max_num + col]->isInvalid();
      if (auto val = values[row * max_num + col])
      {
         bool invalid = false;
         // check horizontal
         for (uint i = 0; i < max_num; ++i)
            if (i != col && values[row * max_num + i] == val)
            {
               cells[row * max_num + i]->setInvalid(true);
               invalid = true;
            }

         // check vertical
         for (uint i = 0; i < max_num; ++i)
            if (i != row && values[i * max_num + col] == val)
            {
               cells[i * max_num + col]->setInvalid(true);
               invalid = true;
            }

         // check block
         for (uint i = 0; i < max_num; ++i)
         {
            uint const r = (row / N) * N + i / N;
            uint const c = (col / N) * N + i % N;

            if ((r != row || c != col) && values[r * max_num + c] == val)
            {
               cells[r * max_num + c]->setInvalid(true);
               invalid = true;
            }
         }
         cells[row * max_num + col]->setInvalid(invalid);
      }

      // if cell was invalid and `with_neighbours` is set
      // try to restore cells that led to invalid state
      if (was_invalid && with_neighbours)
      {
         cells[row * max_num + col]->setInvalid(false);
         for (uint i = 0; i < max_num; ++i)
            if (cells[row * max_num + i]->isInvalid())
               validate(row, i, false);

         for (uint i = 0; i < max_num; ++i)
            if (cells[i * max_num + col]->isInvalid())
               validate(i, col, false);

         for (uint i = 0; i < max_num; ++i)
         {
            uint const r = (row / N) * N + i / N;
            uint const c = (col / N) * N + i % N;

            if (cells[r * max_num + c]->isInvalid())
               validate(r, c, false);
         }
      }
   }

   std::vector<uint> invalidValues(int row, int col)
   {
      std::vector<bool> has(max_num + 1, false);
      for (uint i = 0; i < max_num; ++i)
         has[values[row * max_num + i]] = true;

      for (uint i = 0; i < max_num; ++i)
         has[values[i * max_num + col]] = true;

      for (uint i = 0; i < max_num; ++i)
      {
         uint const r = (row / N) * N + i / N;
         uint const c = (col / N) * N + i % N;
         has[values[r * max_num + c]] = true;
      }
      has[values[row * max_num + col]] = false;

      std::vector<uint> res;
      for (uint i = 1; i <= max_num; ++i)
         if (has[i])
            res.push_back(i);
      return res;
   }

   uint max_num, N;
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
                  d.validate(id / d.max_num, id % d.max_num);
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
                  popup.enableValues(d.invalidValues(id / d.max_num, id % d.max_num), false);
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
         d.validate(row, col);
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

   for (uint i = 0; i != d.cells.size(); ++i)
   {
      auto e = d.cells[i];
      if (reset_readonly || !e->isReadOnly())
      {
         d.values[i] = 0;
         e->setReadOnly(false);
         e->clear();
      }
      e->setInvalid(false);
   }
}
