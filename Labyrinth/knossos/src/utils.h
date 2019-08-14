#pragma once

#include <knossos/labyrinth.h>


namespace knossos
{
   direction_t opposite_direction( direction_t dir )
   {
      return direction_t((int(dir) + 2) % 4);
   }

   position_t move( position_t const & pos, direction_t dir )
   {
      position_t res(pos);
      switch (dir)
      {
      case dir_left:  --res.x; break;
      case dir_right: ++res.x; break;
      case dir_up:    ++res.y; break;
      case dir_down:  --res.y; break;
      default:
         assert(false);
         break;
      }
      return res;
   }

   bool operator == ( position_t const & p1, position_t const & p2 )
   {
      return p1.x == p2.x && p2.x == p2.y;
   }
}
