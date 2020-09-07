#pragma once

#include <string>
#include <string_view>
#include <vector>


struct long_number_t
{
   long_number_t() = default;
   long_number_t(long_number_t const &) = default;
   long_number_t(long_number_t &&) = default;
   long_number_t(long long);

   std::string to_string() const;
   static long_number_t from_string(std::string_view);

   long_number_t operator -() const &;
   long_number_t operator -() &&;

   long_number_t operator + (long_number_t const &) const;
   long_number_t operator - (long_number_t const &) const;
   long_number_t operator * (long_number_t const &) const;

   long_number_t & operator = (long_number_t const &) = default;
   long_number_t & operator = (long_number_t &&) = default;

   long_number_t & operator += (long_number_t const &);
   long_number_t & operator -= (long_number_t const &);
   long_number_t & operator *= (long_number_t const &);

   bool operator < (long_number_t const &) const;
   bool operator > (long_number_t const &) const;
   bool operator == (long_number_t const &) const;
   bool operator != (long_number_t const &) const;

   bool is_null() const;
   void swap(long_number_t &);

private:
   typedef std::vector<uint8_t> digits_t;
   long_number_t(digits_t const &, bool);
   long_number_t(digits_t &&, bool);

private:
   digits_t digits_; // store digit [0..99]
   bool negative_ = false;
};
