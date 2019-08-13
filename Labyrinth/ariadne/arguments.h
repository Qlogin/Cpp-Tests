#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

namespace fs = boost::filesystem;

struct arguments_t
{
   fs::path    board_path;
   std::string route;
   int         x0 = 0;
   int         y0 = 0;
};

boost::optional<arguments_t> parse_arguments( int argc, char * argv[] );
