#include "SudokuWidget.h"

#include <QGridLayout>
#include <QFrame>
#include <QLineEdit>
#include <QValidator>

#include <QKeyEvent>


namespace
{
   class Validator : public QIntValidator
   {
   public:
      Validator(int min, int max, QObject * parent)
         : QIntValidator(min, max, parent)
      {}

   private:
      void fixup(QString & input) const override
      {
         QIntValidator::fixup(input);

         int i = 0;
         while (i != input.size() && input[i] == '0')
            ++i;
         if (i > 0)
            input = input.mid(i);
      }

      QValidator::State validate(QString & input, int & pos) const override
      {
         if (input.startsWith('0'))
         {
            pos = 0;
            return QValidator::Invalid;
         }
         return QIntValidator::validate(input, pos);
      }
   };

   class Edit : public QLineEdit
   {
   public:
      Edit(uint max_num, QWidget * parent = nullptr)
         : QLineEdit(parent)
      {
         QFont font;
         font.setPointSize(15);
         setFont(font);
         setValidator(new Validator(1, max_num, this));
         setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
         setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
      }

   private:
      QSize sizeHint() const override
      {
         return QSize(40, 40);
      }

      void keyPressEvent(QKeyEvent * e)
      {
         if (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right ||
             e->key() == Qt::Key_Up   || e->key() == Qt::Key_Down)
         {
            e->ignore();
            return;
         }
         QLineEdit::keyPressEvent(e);
      }
   };
}

SudokuWidget::SudokuWidget(QWidget *parent, uint N)
   : QWidget(parent)
   , max_num_(N * N)
{
   setStyleSheet("QLineEdit:focus { background-color: rgb(225, 225, 255); }");
   edits_.resize(max_num_ * max_num_);

   auto layout = new QGridLayout(this);
   layout->setSpacing(1);

   for (size_t i = 0; i != max_num_; ++i)
   {
      auto frame = new QFrame(this);
      frame->setFrameShape(QFrame::Box);

      auto l = new QGridLayout(frame);
      l->setMargin(1);
      l->setSpacing(1);

      for (size_t j = 0; j != max_num_; ++j)
      {
         auto edit = new Edit(max_num_, frame);
         l->addWidget(edit, j / N, j % N);

         int const id = ((i / N) * N + j / N) * max_num_
                      + ((i % N) * N + j % N);
         edits_[id] = edit;
      }
      layout->addWidget(frame, i / N, i % N);
   }
}

SudokuWidget::~SudokuWidget()
{
}

void SudokuWidget::keyPressEvent( QKeyEvent * e )
{
   int const N = max_num_ * max_num_;
   int i = N;
   while (i > 0)
   {
      --i;
      if (edits_[i]->hasFocus())
         break;
   }

   switch (e->key())
   {
   case Qt::Key_Left:   i -= 1; break;
   case Qt::Key_Right:  i += 1; break;
   case Qt::Key_Up:     i -= max_num_; break;
   case Qt::Key_Down:   i += max_num_; break;
   default:
      {
         QWidget::keyPressEvent(e);
         return;
      }
   }

   edits_[(i + N) % N]->setFocus();
   e->accept();
}

uint SudokuWidget::value(uint row, uint col) const
{
   if (row < max_num_ || col < max_num_)
   {
      QString const val = edits_[row * max_num_ + col]->text();
      if (!val.isEmpty())
         return val.toUInt();
   }
   return 0;
}

void SudokuWidget::setValue(uint row, uint col, uint value)
{
   if (row < max_num_ || col < max_num_)
   {
      auto edit = edits_[row * max_num_ + col];
      if (0 < value && value <= max_num_)
         edit->setText(QString::number(value));
      else
         edit->clear();
   }
}

void SudokuWidget::clear()
{
   for (auto e : edits_)
      e->clear();
}
