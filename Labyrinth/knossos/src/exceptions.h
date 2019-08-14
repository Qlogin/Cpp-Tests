#pragma once

#include <knossos/labyrinth.h>


namespace knossos
{
   struct position_not_set_error_t : position_error_t
   {
      const char *what() const noexcept override
      {
         return "position is not set";
      }
   };

   struct incorrect_position_error_t : position_error_t
   {
      const char *what() const noexcept override
      {
         return "incorrect start position";
      }
   };
}
