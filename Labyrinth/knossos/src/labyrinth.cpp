#include <knossos/labyrinth.h>

#include <set>
#include <exception>
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

      direction_t opposite_direction( direction_t dir )
      {
         return direction_t((int(dir) + 2) % 4);
      }

      position_t move( position_t const & pos, direction_t dir )
      {
         position_t res(pos);
         switch (dir)
         {
         case direction_t::left:  --res.x; break;
         case direction_t::right: ++res.x; break;
         case direction_t::up:    ++res.y; break;
         case direction_t::down:  --res.y; break;
         default:
            assert(false);
            break;
         }
         return res;
      }
   }

   struct labyrinth_t::impl_t
   {
      section_t const * find_section(position_t const & pos) const
      {
         auto itr = sections.find(pos);
         if (itr == sections.end())
            return nullptr;
         return &(*itr);
      }

      std::set<section_t, section_compare_t> sections;
      boost::optional<position_t>            position;
   };

   ////////////////////////////////////////////////////////////////////////////

   labyrinth_t::labyrinth_t()
      : pimpl_(new impl_t)
   {}

   labyrinth_t::labyrinth_t( sections_range_t sections,
                             boost::optional<position_t> const & start_pos )
      : labyrinth_t()
   {
      add_sections(sections);
      if (start_pos)
         if (!set_position(*start_pos))
            throw std::runtime_error("incorrect start position");
   }

   labyrinth_t::~labyrinth_t()
   {}

   void labyrinth_t::add_sections(sections_range_t sections)
   {
      for (position_t const & pos : sections)
      {
         auto result = pimpl_->sections.emplace(pos);
         if (result.second)
         {
            for (auto const & dir : {left, right, down, up})
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
      // TODO:
   }

   bool labyrinth_t::set_position(position_t const & position)
   {
      if (!pimpl_->find_section(position))
         return false;

      pimpl_->position = position;
      return true;
   }

   boost::optional<position_t> const & labyrinth_t::get_position() const
   {
      return pimpl_->position;
   }

   void labyrinth_t::navigate( directions_range_t route )
   {
      if (!pimpl_->position)
         throw std::runtime_error("position is not set");

      auto cur_section = pimpl_->find_section(*pimpl_->position);
      assert(cur_section);

      for (auto dir : route)
      {
         if (auto next = cur_section->neigbours[dir])
            cur_section = next;
      }
      pimpl_->position = static_cast<position_t const &>(*cur_section);
   }
}
