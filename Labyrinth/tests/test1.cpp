#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <knossos/labyrinth.h>

size_t const num_sections = 4;
static knossos::position_t sections[num_sections] =
{
   {0, 0},
   {0, 1},
   {1, 0},
   {1, 1}
};

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(testSuiteLabyrinth)

BOOST_AUTO_TEST_CASE(testPosition)
{
   knossos::position_t const valid_pos{0, 0};
   knossos::position_t const invalid_pos{-1, -1};
   knossos::labyrinth_t lab(sections);

   BOOST_CHECK_EQUAL(lab.is_position_set(), false);
   BOOST_CHECK_EQUAL(lab.set_position(invalid_pos), false);
   BOOST_CHECK_THROW(lab.position(), knossos::position_error_t);
   BOOST_CHECK(lab.set_position(valid_pos));

   BOOST_CHECK_THROW(knossos::labyrinth_t(sections, invalid_pos),
                     knossos::position_error_t);
}

BOOST_AUTO_TEST_CASE(testSections)
{
   knossos::labyrinth_t lab(sections, knossos::position_t{0, 0});
   BOOST_CHECK(num_sections == boost::size(lab.sections()));

   lab.add_sections(sections);
   BOOST_CHECK(num_sections == boost::size(lab.sections()));

   lab.remove_sections(sections);
   BOOST_CHECK_EQUAL(lab.is_position_set(), false);
}

BOOST_AUTO_TEST_CASE(testRoute)
{
   knossos::position_t const invalid_pos{-1, -1};
   knossos::position_t const start_pos{0, 0};
   knossos::position_t const end_pos{0, 1};
   knossos::labyrinth_t lab(sections);

   knossos::direction_t route[] = {knossos::dir_right,
                                   knossos::dir_up,
                                   knossos::dir_left};
   BOOST_CHECK_THROW(lab.navigate(route), knossos::position_error_t);
   BOOST_CHECK_THROW(lab.navigate(route, invalid_pos), knossos::position_error_t);

   auto pos = lab.navigate(route, start_pos);
   BOOST_CHECK(pos.x == end_pos.x && pos.y == end_pos.y);

   knossos::direction_t route2[]= { knossos::dir_up };
   lab.navigate(route2); // No move
   BOOST_CHECK(pos.x == end_pos.x && pos.y == end_pos.y);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////
