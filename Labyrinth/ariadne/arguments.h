#pragma once

#include <boost/optional.hpp>

struct arguments_t
{
   std::string board_path;
   std::string route;
   int         x0 = 0;
   int         y0 = 0;
   std::string output_path;
};

boost::optional<arguments_t> parse_arguments( int argc, char * argv[] );
