#include <long_number.h>

#include <iostream>


std::string to_string(bool b)
{
   return b ? "true" : "false";
}

std::string to_string(long_number_t const & x)
{
   return x.to_string();
}

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
   std::cout << "Arbitrary-precision arithmetic" << std::endl
             << "Enter two numbers (0 0 - for exit):" << std::endl;

   while (true)
   {
      std::cout << ">> ";
      std::string s1, s2;
      std::cin >> s1 >> s2;
      
      try
      {
         auto x1 = long_number_t::from_string(s1);
         auto x2 = long_number_t::from_string(s2);

         auto print = [&] ( char op, auto const & res )
         {
            std::cout << x1.to_string() << " " << op << " " << x2.to_string() 
                      << " is " << to_string(res) << std::endl;
         };

         print('+', x1 + x2);
         print('-', x1 - x2);
         print('*', x1 * x2);
         print('<', x1 < x2);
         print('>', x1 > x2);
         print('=', x1 == x2);

         if (x1.is_null() && x2.is_null())
            break;
      }
      catch(const std::exception& e)
      {
         std::cout << "Error: " << e.what() << std::endl;
      }
   }
   return 0;
}
