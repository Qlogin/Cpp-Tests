#include "utils.h"
#include "exceptions.h"

#include <set>
#include <assert.h>


namespace knossos
{
   namespace
   {
      struct section_t : position_t
      {
         section_t( position_t const & pos )
            : position_t(pos)
            , neigbours{{}}
         {}

         section_t( int x = 0, int y = 0 )
            : section_t(position_t{x, y})
         {}

         typedef
            std::array<section_t const *, direction_t::total_num>
            neigbours_t;

         mutable neigbours_t neigbours;
      };

      struct section_compare_t
      {
         using is_transparent = void;

         bool operator() (section_t const &ls, section_t const &rs) const
         {
            return ls.x < rs.x || (ls.x == rs.x && ls.y < rs.y);
         }
      };
   }

   struct labyrinth_t::impl_t
   {
      std::set<section_t, section_compare_t> sections;
      section_t const * current_pos = nullptr;

      section_t const * find_section(position_t const & pos) const
      {
         auto itr = sections.find(pos);
         if (itr == sections.end())
            return nullptr;
         return &(*itr);
      }
   };

   ////////////////////////////////////////////////////////////////////////////

   labyrinth_t::labyrinth_t()
      : pimpl_(new impl_t)
   {}

   labyrinth_t::labyrinth_t(sections_range_t sections,
                            boost::optional<position_t> const & start_pos)
      : labyrinth_t()
   {
      add_sections(sections);
      if (start_pos)
         if (!set_position(*start_pos))
            throw incorrect_position_error_t();
   }

   labyrinth_t::~labyrinth_t()
   {}

   void labyrinth_t::add_sections(sections_range_t sections)
   {
      for (position_t pos : sections)
      {
         auto result = pimpl_->sections.emplace(pos);
         if (result.second)
         {
            for (auto dir : {dir_left, dir_right, dir_down, dir_up})
            {
               if (auto section = pimpl_->find_section(move(pos, dir)))
               {
                  result.first->neigbours[dir] = section;
                  section->neigbours[opposite_direction(dir)] = &(*result.first);
               }
            }
         }
      }
   }

   void labyrinth_t::remove_sections(sections_range_t sections)
   {
      for (position_t pos : sections)
      {
         auto itr = pimpl_->sections.find(pos);
         if (itr != pimpl_->sections.end())
         {
            for (auto dir : {dir_left, dir_right, dir_down, dir_up})
               if (auto neigbour = itr->neigbours[dir])
                  neigbour->neigbours[opposite_direction(dir)] = nullptr;

            if (pimpl_->current_pos == &(*itr))
               pimpl_->current_pos = nullptr;

            pimpl_->sections.erase(itr);
         }
      }
   }

   bool labyrinth_t::set_position(position_t const & position)
   {
      if (auto section = pimpl_->find_section(position))
      {
         pimpl_->current_pos = section;
         return true;
      }
      return false;
   }

   position_t const & labyrinth_t::get_position() const
   {
      if (pimpl_->current_pos)
         throw position_not_set_error_t();

      return *pimpl_->current_pos;
   }

   void labyrinth_t::navigate(directions_range_t route,
                              boost::optional<position_t> const & start_pos)
   {
      if (start_pos && !set_position(*start_pos))
         throw incorrect_position_error_t();
      if (!pimpl_->current_pos)
         throw position_not_set_error_t();

      for (auto dir : route)
         if (auto next = pimpl_->current_pos->neigbours[dir])
            pimpl_->current_pos = next;
   }
}
