#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

// Do not use any header files other than the following
#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

namespace sjtu {

class int2048 {
private:
  int sign; // 1 for >= 0, -1 for < 0
  std::vector<int> a; // digits in base 10^9 (little endian: a[0] is lowest)

  static const int BASE = 1000000000;
  static const int BASE_DIGITS = 9;

  void trim();
  static int compare_abs(const int2048 &x, const int2048 &y);
  static int2048 add_abs(const int2048 &x, const int2048 &y);
  static int2048 sub_abs(const int2048 &x, const int2048 &y); // assumes |x| >= |y|
  static int2048 mul_small(const int2048 &x, const int2048 &y);
  static int2048 mul_ntt(const int2048 &x, const int2048 &y);
  static int2048 div_knuth(const int2048 &x, const int2048 &y, int2048 &r);
  static int2048 div_newton(const int2048 &x, const int2048 &y, int2048 &r);
  static void div_mod_abs(const int2048 &x, const int2048 &y, int2048 &q, int2048 &r);

public:
  // Constructors
  int2048();
  int2048(long long);
  int2048(const std::string &);
  int2048(const int2048 &);

  // Integer1
  void read(const std::string &);
  void print();

  int2048 &add(const int2048 &);
  friend int2048 add(int2048, const int2048 &);

  int2048 &minus(const int2048 &);
  friend int2048 minus(int2048, const int2048 &);

  // Integer2
  int2048 operator+() const;
  int2048 operator-() const;

  int2048 &operator=(const int2048 &);

  int2048 &operator+=(const int2048 &);
  friend int2048 operator+(int2048, const int2048 &);

  int2048 &operator-=(const int2048 &);
  friend int2048 operator-(int2048, const int2048 &);

  int2048 &operator*=(const int2048 &);
  friend int2048 operator*(int2048, const int2048 &);

  int2048 &operator/=(const int2048 &);
  friend int2048 operator/(int2048, const int2048 &);

  int2048 &operator%=(const int2048 &);
  friend int2048 operator%(int2048, const int2048 &);

  friend std::istream &operator>>(std::istream &, int2048 &);
  friend std::ostream &operator<<(std::ostream &, const int2048 &);

  friend bool operator==(const int2048 &, const int2048 &);
  friend bool operator!=(const int2048 &, const int2048 &);
  friend bool operator<(const int2048 &, const int2048 &);
  friend bool operator>(const int2048 &, const int2048 &);
  friend bool operator<=(const int2048 &, const int2048 &);
  friend bool operator>=(const int2048 &, const int2048 &);
};

} // namespace sjtu

#endif
