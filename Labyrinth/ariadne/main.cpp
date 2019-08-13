#include "arguments.h"
#include <knossos/labyrinth.h>

#include <iostream>


int main(int argc, char *argv[])
{
   try
   {
      auto args = parse_arguments(argc, argv);
      if (!args)
         return 0;

      knossos::labyrinth_t lab;
      // TODO: lab = knossos::load_from_file(arguments.board_path);

      if (!lab.set_position(knossos::position_t{args->x0, args->y0}))
      {
         std::cerr << "incorrect start position: " << args->x0 << " " << args->y0 << std::endl;
         return 1;
      }

      std::vector<knossos::direction_t> route;
      route.reserve((args->route.size()));

      for (auto const & ch : args->route)
      {
         switch (ch)
         {
         case 'u': route.push_back(knossos::direction_t::up); break;
         case 'd': route.push_back(knossos::direction_t::down); break;
         case 'l': route.push_back(knossos::direction_t::left); break;
         case 'r': route.push_back(knossos::direction_t::right); break;
         default:
            {
               std::cerr << "invalid route symbol: " << ch << std::endl;
               return 1;
            }
         }
      }
      lab.navigate(route);

      auto pos = lab.get_position();
      if (!pos)
         throw std::runtime_error("Something goes wrong!");

      std::cout << "(" << pos->x << "," << pos->y << ")" << std::endl;
      return 0;
   }
   catch (std::exception const & e)
   {
      std::cerr << "error: " << e.what() << std::endl;
      return 1;
   }
}
