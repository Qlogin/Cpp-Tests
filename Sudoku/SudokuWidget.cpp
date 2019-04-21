#include "SudokuWidget.h"

#include <QGridLayout>
#include <QFrame>
#include <QLineEdit>
#include <QValidator>
#include <QKeyEvent>

#include <vector>

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
            input = input.mid(i).trimmed();
      }

      QValidator::State validate(QString & input, int & pos) const override
      {
         if (input.startsWith('0'))
         {
            pos = 0;
            return QValidator::Invalid;
         }

         auto space = input.indexOf(' ');
         if (space != -1)
         {
            pos = space;
            return QValidator::Invalid;
         }

         return QIntValidator::validate(input, pos);
      }
   };

   static const QColor active_color(220, 220, 255);
   static const QColor readonly_color(200, 200, 200);

   class Edit : public QLineEdit
   {
   public:
      Edit(uint max_num, QWidget * parent = nullptr)
         : QLineEdit(parent)
         , normal_color_(palette().color(QPalette::Base))
      {
         QFont font;
         font.setPointSize(15);
         setFont(font);

         setValidator(new Validator(1, int(max_num), this));
         setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
         setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
      }

      void setReadOnly( bool readonly )
      {
         QLineEdit::setReadOnly(readonly);
         setBackground(readonly   ? readonly_color :
                       hasFocus() ? active_color :
                                    normal_color_);
      }

   private:
      void setBackground( QColor const & color )
      {
         QPalette pal = palette();
         pal.setColor(QPalette::Base, color);
         setPalette(pal);
      }

   private:
      QSize sizeHint() const override
      {
         return QSize(40, 40);
      }

      void keyPressEvent(QKeyEvent * e) override
      {
         if (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right ||
             e->key() == Qt::Key_Up   || e->key() == Qt::Key_Down)
         {
            e->ignore();
            return;
         }
         QLineEdit::keyPressEvent(e);
      }

      void focusInEvent(QFocusEvent * e) override
      {
         if (!isReadOnly())
            setBackground(active_color);
         QLineEdit::focusInEvent(e);
      }

      void focusOutEvent(QFocusEvent * e) override
      {
         if (!isReadOnly())
            setBackground(normal_color_);
         QLineEdit::focusOutEvent(e);
      }

   private:
      QColor const normal_color_;
   };
}

struct SudokuWidget::SudokuWidgetPrivate
{
   SudokuWidgetPrivate(uint N)
      : max_num(N * N)
   {
      values.resize(max_num * max_num, 0);
      edits.resize(values.size(), nullptr);
   }

   uint max_num;
   std::vector<uint> values;
   std::vector<Edit *> edits;
};

SudokuWidget::SudokuWidget(QWidget *parent, uint N)
   : QWidget(parent)
   , pimpl_(new SudokuWidgetPrivate(N))
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
         auto edit = new Edit(d.max_num, frame);
         l->addWidget(edit, j / N, j % N);

         uint const id = ((i / N) * N + j / N) * d.max_num
                       + ((i % N) * N + j % N);
         d.edits[id] = edit;

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
      if (d.edits[i]->hasFocus())
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
   } while (i < N && d.edits[i]->isReadOnly());

   if (i < N)
      d.edits[i]->setFocus();
   e->accept();
}

uint SudokuWidget::value(uint row, uint col) const
{
   auto & d = *pimpl_;

   if (row < d.max_num || col < d.max_num)
      return d.values[row * d.max_num + col];

   return 0;
}

void SudokuWidget::setValue(uint row, uint col, uint value, bool readonly)
{
   auto & d = *pimpl_;

   if (row < d.max_num || col < d.max_num)
   {
      auto const idx = row * d.max_num + col;
      auto edit = d.edits[idx];
      QSignalBlocker block(edit);

      if (0 < value && value <= d.max_num)
      {
         d.values[idx] = value;
         edit->setText(QString::number(value));
         edit->setReadOnly(readonly);
      }
      else
      {
         d.values[idx] = 0;
         edit->setReadOnly(false);
         edit->clear();
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
         d.edits[idx]->setReadOnly(readonly);
   }
}

void SudokuWidget::clear(bool reset_readonly)
{
   auto & d = *pimpl_;

   for (auto e : d.edits)
      if (reset_readonly || !e->isReadOnly())
      {
         e->setReadOnly(false);
         e->clear();
      }

   std::fill(d.values.begin(), d.values.end(), 0);
}
