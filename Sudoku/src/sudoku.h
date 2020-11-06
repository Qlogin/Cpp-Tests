#pragma once

#include <QString>
#include <array>


namespace sudoku
{
   size_t constexpr Dim = 3;
   size_t constexpr Size = Dim * Dim;

   using fields_t = std::array<std::array<uint8_t, Size>, Size>;

   bool load(QString const & filename, fields_t & fields);
   bool save(QString const & filename, fields_t const & fields);
   bool solve(fields_t & fields);
}
