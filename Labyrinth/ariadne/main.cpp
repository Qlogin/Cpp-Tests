#include "arguments.h"
#include "load_sections.h"

#include <boost/format.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <fstream>
#include <iostream>


namespace ba = boost::adaptors;

namespace
{
   knossos::direction_t char_to_dir(char ch)
   {
      switch (ch)
      {
      case 'u': return knossos::dir_up;
      case 'd': return knossos::dir_down;
      case 'l': return knossos::dir_left;
      case 'r': return knossos::dir_right;
      default:
         {
            boost::format error("invalid route character: %1%");
            throw std::runtime_error(str(error % ch));
         }
      }
   }
}

int main(int argc, char *argv[])
{
   try
   {
      auto args = parse_arguments(argc, argv);
      if (!args)
         return 0;

      std::vector<knossos::position_t> sections;
      load_sections(args->board_path, sections);

      knossos::labyrinth_t lab(sections);
      if (!lab.set_position(knossos::position_t{args->x0, args->y0}))
      {
         std::cerr << "incorrect start position: " << args->x0 << " " << args->y0 << std::endl;
         return 1;
      }
      lab.navigate(args->route | ba::transformed(&char_to_dir));

      auto const & pos = lab.position();
      if (args->output_path.empty())
         std::cout << "(" << pos.x << "," << pos.y << ")" << std::endl;
      else
      {
         std::ofstream outf(args->output_path.c_str());
         if (!outf)
         {
            std::cerr << "failed to write file: " << args->output_path << std::endl;
            return 1;
         }
         outf << "(" << pos.x << "," << pos.y << ")";
      }
      return 0;
   }
   catch (std::exception const & e)
   {
      std::cerr << "error: " << e.what() << std::endl;
      return 1;
   }
}
