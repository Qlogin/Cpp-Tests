#include "long_number.h"

#include <assert.h>
#include <tuple>
#include <string_view>
#include <memory>

///////////////////////////////////////////////////////////////////////////////

namespace
{
   static const short BASE = 100;

   typedef std::vector<uint8_t> digits_t;

   inline std::tuple<char, char> to_char(uint8_t digit)
   {
      assert(digit < BASE);
      return {'0' + (digit / 10), '0' + (digit % 10)};
   }

   inline uint8_t from_char(char c1, char c2)
   {
      if (!isdigit(c1) || !isdigit(c2))
         throw std::bad_cast();

      return (c1 - '0') * 10 + (c2 - '0');
   }
   
   inline size_t num_digits(digits_t const & digits)
   {
      if (digits.empty())
         return 0;
      
      size_t const num = 2 * digits.size();
      return digits.back() < 10 ? num - 1 : num;
   }

   inline bool remove_leading_zeros(digits_t & digits)
   {
      while (!digits.empty() && digits.back() == 0)
         digits.pop_back();
      return digits.empty();
   }

   int compare_digits( digits_t const & lhs, digits_t const & rhs )
   {
      if (lhs.size() == rhs.size())
      {
         for (auto lit = lhs.rbegin(),
                   rit = rhs.rbegin(); lit != lhs.rend(); ++lit, ++rit)
         {
            if (*lit != *rit)
               return *lit < *rit ? -1 : 1;
         }
         return 0; // equal
      }
      return lhs.size() < rhs.size() ? -1 : 1;
   }

   digits_t add_digits(digits_t const & first, digits_t const & second)
   {
      digits_t sum(std::max(first.size(), second.size()) + 1);
      for (size_t i = 0; i != first.size(); ++i)
         sum[i] = first[i];

      uint8_t remain = 0;
      size_t i = 0;
      while (i != second.size())
      {
         sum[i] += second[i] + remain;
         remain = (sum[i] < BASE) ? 0 : 1;
         if (remain)
            sum[i] -= BASE;
         ++i;
      }
      while (remain)
      {
         sum[i] += remain;
         remain = (sum[i] < BASE) ? 0 : 1;
         if (remain)
            sum[i] -= BASE;
         ++i;
      }
      remove_leading_zeros(sum);
      return sum;
   }

   digits_t sub_digits( digits_t const & lhs, digits_t const & rhs, bool & negative )
   {
      int const cmp = compare_digits(lhs, rhs);
      if (cmp == 0)
         return digits_t();

      bool const less = (cmp < 0);
      digits_t minuend = less ? rhs : lhs; // copy
      digits_t const & subtrahend = less ? lhs : rhs;

      uint8_t borrow = 0;
      size_t i = 0;
      while (i != subtrahend.size())
      {
         uint8_t const num = subtrahend[i] + borrow;
         if (minuend[i] < num)
         {
            minuend[i] += BASE - num;
            borrow = 1;
         }
         else
         {
            minuend[i] -= num;
            borrow = 0;
         }
         ++i;
      }

      while (borrow && i != minuend.size())
      {
         if (minuend[i] == 0)
            minuend[i] = BASE - borrow;
         else
         {
            minuend[i] -= borrow;
            borrow = 0;
         }
         ++i;
      }
      assert(!borrow);

      if (remove_leading_zeros(minuend))
         negative = false;
      else if (less)
         negative = !negative;
     
      return minuend;
   }
}

///////////////////////////////////////////////////////////////////////////////

long_number_t::long_number_t(long long number)
   : negative_(number < 0)
{
   if (negative_)
      number = -number;
   while (number != 0)
   {
      digits_.push_back(number % BASE);
      number /= BASE;
   }
}

long_number_t::long_number_t(std::vector<uint8_t> const & digits, bool negative)
   : digits_(digits)
   , negative_(negative)
{}

long_number_t::long_number_t(std::vector<uint8_t> && digits, bool negative)
   : digits_(std::move(digits))
   , negative_(negative)
{}

std::string long_number_t::to_string() const
{
   if (digits_.empty())
      return "0";

   size_t pos = num_digits(digits_) + (int)negative_;

   std::string res(pos, ' ');
   for (size_t i = 0, n = digits_.size() - 1; i != n; ++i)
   {
      std::tie(res[pos - 2], res[pos - 1]) = to_char(digits_[i]);
      pos -= 2;
   }

   char first;
   std::tie(first, res[pos - 1]) = to_char(digits_.back());
   if (first != '0')
   {
      assert(pos > 1);
      res[pos - 2] = first;
   }
   if (negative_)
      res[0] = '-';

   return res;
}

/* static */ long_number_t long_number_t::from_string(std::string_view str)
{
   long_number_t res;
   if (!str.empty() && (str[0] == '-' || str[0] == '+'))
   {
      res.negative_ = (str[0] == '-');
      str = str.substr(1);
   }
   if (str.empty())
      throw std::bad_cast();

   res.digits_.reserve((str.size() + 1) / 2);
   for (size_t i = str.size() - 1; i > 1; i -= 2)
      res.digits_.push_back(from_char(str[i - 1], str[i]));

   if (str.size() % 2)
      res.digits_.push_back(from_char('0', str[0]));
   else
      res.digits_.push_back(from_char(str[0], str[1]));

   if (remove_leading_zeros(res.digits_))
      res.negative_ = false;

   return res;
}

long_number_t long_number_t::operator -() const &
{
   return long_number_t{digits_, !negative_};
}

long_number_t long_number_t::operator -() &&
{
   return long_number_t{std::move(digits_), !negative_};
}

long_number_t long_number_t::operator +(long_number_t const & other) const
{
   long_number_t result({}, negative_);
   if (negative_ == other.negative_)
      add_digits(digits_, other.digits_).swap(result.digits_);
   else
      sub_digits(digits_, other.digits_, result.negative_).swap(result.digits_);
   return result;
}

long_number_t long_number_t::operator -(long_number_t const & other) const
{
   long_number_t result({}, negative_);
   if (negative_ != other.negative_)
      add_digits(digits_, other.digits_).swap(result.digits_);
   else
      sub_digits(digits_, other.digits_, result.negative_).swap(result.digits_);
   return result;
}

long_number_t long_number_t::operator *(long_number_t const & other) const
{
   if (is_null() || other.is_null())
      return long_number_t();

   digits_t result((num_digits(digits_) + num_digits(other.digits_) + 1) / 2);

   for (size_t i = 0; i != digits_.size(); ++i)
   {
      short remain = 0;
      size_t j = 0;
      while (j != other.digits_.size())
      {
         auto const mult = result[i + j] + (short)digits_[i] * (short)other.digits_[j] + remain;
         result[i + j] = mult % BASE;
         remain = mult / BASE;
         ++j;
      }
      while (remain)
      {
         auto const mult = result[i + j] + remain;
         result[i + j] = mult % BASE;
         remain = mult / BASE;
         ++j;
      }
   }
   remove_leading_zeros(result);
   return long_number_t{std::move(result), negative_ != other.negative_};
}

long_number_t & long_number_t::operator += (long_number_t const & other)
{
   (*this + other).swap(*this);
   return *this;
}

long_number_t & long_number_t::operator -= (long_number_t const & other)
{
   (*this - other).swap(*this);
   return *this;
}

long_number_t & long_number_t::operator *= (long_number_t const & other)
{
   (*this * other).swap(*this);
   return *this;
}

bool long_number_t::operator < (long_number_t const & other) const
{
   if (negative_ != other.negative_)
      return negative_;

   bool const less = compare_digits(digits_, other.digits_) < 0;
   return negative_ ? !less : less;
}

bool long_number_t::operator > (long_number_t const & other) const
{
   return other < *this;
}

bool long_number_t::operator == (long_number_t const & other) const
{
   return negative_ == other.negative_ 
       && digits_   == other.digits_;
}

bool long_number_t::operator != (long_number_t const & other) const
{
   return !(*this == other);
}

bool long_number_t::is_null() const 
{
   return digits_.empty();
}

void long_number_t::swap(long_number_t & other)
{
   std::swap(digits_, other.digits_);
   std::swap(negative_, other.negative_);
}

long_number_t operator "" _ln(const char * str)
{
   return long_number_t::from_string(str);
}

long_number_t operator "" _ln(const char * str, std::size_t size)
{
   return long_number_t::from_string(std::string_view(str, size));
}
