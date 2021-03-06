#include "sudoku.h"

#include <QFile>
#include <QTextStream>

#include <cmath>


namespace sudoku
{
   bool load(QString const & filename, fields_t & fields)
   {
      QFile file(filename);
      if (!file.open(QFile::ReadOnly))
         return false;

      QTextStream stream(&file);

      for (auto & row : fields)
      {
         QString line;
         while (stream.readLineInto(&line) && line.trimmed().startsWith('#'))
            ;

         if (line.length() != sudoku::Size)
            return false;

         for (uint i = 0; i != Size; ++i)
         {
            if (line[i].isPunct())
               row[i] = 0;
            else if (line[i].isDigit())
               row[i] = line[i].digitValue();
            else
               return false;
         }
      }
      return true;
   }

   bool save(QString const & filename, fields_t const & fields)
   {
      QFile file(filename);
      if (!file.open(QFile::WriteOnly|QFile::Truncate))
         return false;

      QTextStream stream(&file);
      stream << QString::fromUtf8("# Simple Sudoku Format") << endl;

      for (auto const & row : fields)
      {
         QString line;
         for (auto const & val : row)
         {
            if (val)
               line += QString::number(val);
            else
               line += '.';
         }
         stream << line << endl;
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////

   namespace
   {
      struct solution_t
      {
         bool solve( fields_t & fields )
         {
            if (!init(fields))
               return false;

            return solve_iter(fields, 0);
         }

      private:
         bool init(fields_t const & fields)
         {
            for (uint i = 0; i < Size; ++i)
               for (uint j = 0; j < Size; ++j)
                  if (auto val = fields[i][j])
                  {
                     if (!can_use(i, j, val))
                        return false;
                     set_used(i, j, val, true);
                  }
            return true;
         }

         void set_used(uint i, uint j, uint8_t val, bool used)
         {
            hor_[i][val] = used;
            vert_[j][val] = used;

            uint const qi = i / Dim, qj = j / Dim;
            quad_[qi * Dim + qj][val] = used;
         }

         bool can_use(uint i, uint j, uint8_t val) const
         {
            uint const qi = i / Dim, qj = j / Dim;
            return !hor_[i][val] && !vert_[j][val] && !quad_[qi * Dim + qj][val];
         }

         bool solve_iter(fields_t & fields, uint step)
         {
            static constexpr uint last_step = Size * Size;

            while (step != last_step)
            {
               uint const i = step / Size;
               uint const j = step % Size;

               if (!fields[i][j])
               {
                  for (uint8_t val = 1; val <= Size; ++val)
                     if (can_use(i, j, val))
                     {
                        fields[i][j] = val;
                        set_used(i, j, val, true);
                        if (solve_iter(fields, step + 1))
                           return true;
                        set_used(i, j, val, false);
                        fields[i][j] = 0;
                     }
                  return false;
               }
               ++step;
            }
            return step == last_step;
         }

      private:
         using NumberSet = std::array<std::array<bool, Size + 1>, Size>;
         NumberSet hor_, vert_, quad_;
      };
   }

   bool solve( fields_t & fields )
   {
      return solution_t().solve(fields);
   }
}
