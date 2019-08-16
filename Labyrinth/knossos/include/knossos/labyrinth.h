#pragma once

#include <knossos/export.h>

#include <boost/range/any_range.hpp>
#include <boost/optional.hpp>

#include <memory>
#include <exception>


namespace knossos
{
   struct position_t
   {
      int x = 0, y = 0;
   };

   enum direction_t
   {
      dir_up,
      dir_left,
      dir_down,
      dir_right,

      total_num
   };

   template <class Value, class Tag = boost::bidirectional_traversal_tag>
   struct values_range_t
   {
      typedef
         boost::any_range<const Value, Tag, const Value>
         type;
   };

   typedef
      values_range_t<direction_t>::type
      directions_range_t;

   typedef
      values_range_t<position_t>::type
      positions_range_t;

   ////////////////////////////////////////////////////////////////////////////

   struct position_error_t : std::exception
   {
   };

   class KNOSSOS_EXPORT labyrinth_t
   {
   public:
      labyrinth_t();
      labyrinth_t(positions_range_t sections,
                  boost::optional<position_t> const & start_position = boost::none);
      ~labyrinth_t();

      void add_sections(positions_range_t sections);
      void remove_sections(positions_range_t sections);
      positions_range_t sections() const;

      bool is_position_set() const;
      bool set_position(position_t const & position);
      position_t const & position() const;

      position_t const & navigate(directions_range_t route,
         boost::optional<position_t> const & start_position = boost::none);
   private:
      struct impl_t;
      std::unique_ptr<impl_t> pimpl_;
   };
}
