#include "load_sections.h"

#include <boost/format.hpp>
#include <fstream>


namespace fs = boost::filesystem;
namespace
{
   struct file_istream_t
   {
      file_istream_t( fs::path const & filepath )
         : stream_(filepath.string().c_str())
         , filename_(filepath.filename().string())
      {
         if (!stream_.is_open())
         {
            boost::format error("failed to open file '%1%'");
            throw std::runtime_error(str(error % filepath));
         }
      }

      explicit operator bool() const
      {
         return !stream_.eof();
      }

      void read_character( char expected, bool check_eof = false )
      {
         char ch = 0;
         stream_ >> ch;

         if (ch != expected)
            if (!(check_eof && stream_.eof()))
               throw_expected({ '\'', expected, '\''});
      }

      void read_number( int & num )
      {
         stream_ >> num;
         if (!stream_.good())
            throw_expected("number");
      }

   private:
      [[ noreturn ]] void throw_expected(std::string const & expected)
      {
         boost::format error("%1%(%2%): invalid input, expected %3%");
         throw std::runtime_error(
            str(error % filename_ % stream_.tellg() % expected)
         );
      }

   private:
      std::ifstream stream_;
      std::string const filename_;
   };
}

void load_sections(fs::path const & filepath,
                   std::vector<knossos::position_t> & sections)
{
   sections.clear();

   file_istream_t file(filepath);
   while (file)
   {
      knossos::position_t pos;

      file.read_character('(');
      file.read_number(pos.x);
      file.read_character(',');
      file.read_number(pos.y);
      file.read_character(')');
      file.read_character(',', true);

      sections.push_back(pos);
   }
}
