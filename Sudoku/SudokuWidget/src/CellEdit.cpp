#include "CellEdit.h"
#include "SudokuWidget.h"

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
            input = input.mid(i).trimmed();
      }

      QValidator::State validate(QString & input, int & pos) const override
      {
         if (input.startsWith('0') || input.startsWith('+'))
         {
            pos = 0;
            return Invalid;
         }

         auto space = input.indexOf(' ');
         if (space != -1)
         {
            pos = space;
            return Invalid;
         }

         auto const res = QIntValidator::validate(input, pos);
         if (res == Intermediate)
            if (input.toInt() > top())
               return Invalid;
         return res;
      }
   };
}

/////////////////////////////////////////////////////////////////////

CellEdit::CellEdit(uint max_num, Palette const & pal, QWidget * parent)
   : QLineEdit(parent)
   , palette_(pal)
{
   QFont font;
   font.setPointSize(15);
   setFont(font);

   setValidator(new Validator(1, int(max_num), this));
   setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
   setMinimumSize(30, 30);
   setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void CellEdit::setReadOnly( bool readonly )
{
   QLineEdit::setReadOnly(readonly);
   update();
}

void CellEdit::setInvalid( bool invalid )
{
   if (invalid_ != invalid)
   {
      invalid_ = invalid;
      update();
   }
}

bool CellEdit::isInvalid() const
{
   return invalid_;
}

void CellEdit::update()
{
   setBackground(isReadOnly() ? palette_[SudokuWidget::Readonly] :
                 invalid_     ? palette_[SudokuWidget::Invalid]  :
                 hasFocus()   ? palette_[SudokuWidget::Active]   :
                                palette_[SudokuWidget::Common]);
}

void CellEdit::setBackground( QColor const & color )
{
   QPalette pal = palette();
   pal.setColor(QPalette::Base, color);
   setPalette(pal);
}

QSize CellEdit::sizeHint() const
{
   return QSize(40, 40);
}

void CellEdit::keyPressEvent(QKeyEvent * e)
{
   if (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right ||
       e->key() == Qt::Key_Up   || e->key() == Qt::Key_Down)
   {
      e->ignore();
      return;
   }

   if (e->key() == Qt::Key_Delete)
   {
      clear();
      e->accept();
      return;
   }

   QLineEdit::keyPressEvent(e);
}

void CellEdit::focusInEvent(QFocusEvent * e)
{
   if (!isReadOnly() && !invalid_)
      setBackground(palette_[SudokuWidget::Active]);
   QLineEdit::focusInEvent(e);
}

void CellEdit::focusOutEvent(QFocusEvent * e)
{
   if (!isReadOnly() && !invalid_)
      setBackground(palette_[SudokuWidget::Common]);
   QLineEdit::focusOutEvent(e);
}
