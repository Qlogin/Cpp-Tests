#include "arguments.h"

#include <boost/program_options.hpp>

#include <iostream>


boost::optional<arguments_t> parse_arguments( int argc, char * argv[] )
{
   namespace po = boost::program_options;
   po::options_description descr("Program options");

   arguments_t parsed;
   descr.add_options()
      ("help,h"  , "display this help and exit")
      ("board"   , po::value<std::string>(&parsed.board_path)->required(),
         "specify path to file with labyrinth")
      ("route"   , po::value<std::string>(&parsed.route)->required(),
         "describe route in format /[dlru]+/")
      ("x,x"     , po::value<int>(&parsed.x0)->default_value(0),
         "start position x-coordinate")
      ("y,y"     , po::value<int>(&parsed.y0)->default_value(0),
         "start position y-coordinate")
      ("output,o", po::value<std::string>(&parsed.output_path),
         "output result to specified file (instead of stdout)")
      ;

   auto print_usage = [&descr]
   {
      std::cout << "Usage: ariadne [options]..." << std::endl;
      std::cout << descr << std::endl;
   };

   try
   {
      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).options(descr).run(), vm);

      if (vm.count("help"))
      {
         print_usage();
         return boost::none;
      }
      po::notify(vm);
   }
   catch (...)
   {
      print_usage();
      throw;
   }
   return boost::make_optional(parsed);
}
