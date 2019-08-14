#pragma once

#include <knossos/labyrinth.h>
#include <boost/filesystem/path.hpp>
#include <vector>


void load_sections(boost::filesystem::path const & path,
                   std::vector<knossos::position_t> & sections);
