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

// Implementations

inline void int2048::trim() {
  while (a.size() > 1 && a.back() == 0) {
    a.pop_back();
  }
  if (a.empty()) {
    a.push_back(0);
    sign = 1;
  }
  if (a.size() == 1 && a[0] == 0) {
    sign = 1;
  }
}

inline int2048::int2048() : sign(1), a(1, 0) {}

inline int2048::int2048(long long v) {
  if (v < 0) {
    sign = -1;
    v = -v;
  } else {
    sign = 1;
  }
  if (v == 0) {
    a.push_back(0);
  } else {
    while (v > 0) {
      a.push_back(v % BASE);
      v /= BASE;
    }
  }
}

inline int2048::int2048(const std::string &s) {
  read(s);
}

inline int2048::int2048(const int2048 &o) : sign(o.sign), a(o.a) {}

inline void int2048::read(const std::string &s) {
  a.clear();
  sign = 1;
  if (s.empty()) {
    a.push_back(0);
    return;
  }
  size_t start = 0;
  if (s[0] == '-') {
    sign = -1;
    start = 1;
  } else if (s[0] == '+') {
    start = 1;
  }
  while (start < s.size() && s[start] == '0') {
    start++;
  }
  if (start == s.size()) {
    sign = 1;
    a.push_back(0);
    return;
  }
  for (int i = (int)s.size(); i > (int)start; i -= BASE_DIGITS) {
    int val = 0;
    int begin = std::max((int)start, i - BASE_DIGITS);
    for (int j = begin; j < i; j++) {
      val = val * 10 + (s[j] - '0');
    }
    a.push_back(val);
  }
  trim();
}

inline void int2048::print() {
  if (sign == -1 && !(a.size() == 1 && a[0] == 0)) {
    putchar('-');
  }
  printf("%d", a.empty() ? 0 : a.back());
  for (int i = (int)a.size() - 2; i >= 0; i--) {
    printf("%09d", a[i]);
  }
}

inline int int2048::compare_abs(const int2048 &x, const int2048 &y) {
  if (x.a.size() != y.a.size()) {
    return x.a.size() < y.a.size() ? -1 : 1;
  }
  for (int i = (int)x.a.size() - 1; i >= 0; i--) {
    if (x.a[i] != y.a[i]) {
      return x.a[i] < y.a[i] ? -1 : 1;
    }
  }
  return 0;
}

inline int2048 int2048::add_abs(const int2048 &x, const int2048 &y) {
  int2048 res;
  res.a.clear();
  long long carry = 0;
  size_t n = std::max(x.a.size(), y.a.size());
  for (size_t i = 0; i < n || carry; i++) {
    long long sum = carry;
    if (i < x.a.size()) sum += x.a[i];
    if (i < y.a.size()) sum += y.a[i];
    if (sum >= BASE) {
      carry = sum / BASE;
      sum %= BASE;
    } else {
      carry = 0;
    }
    res.a.push_back((int)sum);
  }
  res.sign = 1;
  res.trim();
  return res;
}

inline int2048 int2048::sub_abs(const int2048 &x, const int2048 &y) {
  int2048 res;
  res.a.clear();
  long long borrow = 0;
  for (size_t i = 0; i < x.a.size(); i++) {
    long long diff = (long long)x.a[i] - borrow - (i < y.a.size() ? y.a[i] : 0);
    if (diff < 0) {
      diff += BASE;
      borrow = 1;
    } else {
      borrow = 0;
    }
    res.a.push_back((int)diff);
  }
  res.sign = 1;
  res.trim();
  return res;
}

// ===================================
// Integer1
// ===================================

inline int2048 &int2048::add(const int2048 &o) {
  if (sign == o.sign) {
    int2048 res = add_abs(*this, o);
    res.sign = sign;
    *this = res;
  } else {
    int cmp = compare_abs(*this, o);
    if (cmp >= 0) {
      int orig_sign = sign;
      *this = sub_abs(*this, o);
      sign = orig_sign;
    } else {
      int orig_sign = o.sign;
      *this = sub_abs(o, *this);
      sign = orig_sign;
    }
  }
  trim();
  return *this;
}

inline int2048 add(int2048 a, const int2048 &b) {
  return a.add(b);
}

inline int2048 &int2048::minus(const int2048 &o) {
  if (sign != o.sign) {
    int2048 res = add_abs(*this, o);
    res.sign = sign;
    *this = res;
  } else {
    int cmp = compare_abs(*this, o);
    if (cmp >= 0) {
      int orig_sign = sign;
      *this = sub_abs(*this, o);
      sign = orig_sign;
    } else {
      int orig_sign = -sign;
      *this = sub_abs(o, *this);
      sign = orig_sign;
    }
  }
  trim();
  return *this;
}

inline int2048 minus(int2048 a, const int2048 &b) {
  return a.minus(b);
}

// ===================================
// Integer2
// ===================================

inline int2048 int2048::operator+() const {
  return *this;
}

inline int2048 int2048::operator-() const {
  int2048 res(*this);
  if (!(res.a.size() == 1 && res.a[0] == 0)) {
    res.sign = -res.sign;
  }
  return res;
}

inline int2048 &int2048::operator=(const int2048 &o) {
  if (this != &o) {
    sign = o.sign;
    a = o.a;
  }
  return *this;
}

inline int2048 &int2048::operator+=(const int2048 &o) {
  return add(o);
}

inline int2048 operator+(int2048 a, const int2048 &b) {
  return a.add(b);
}

inline int2048 &int2048::operator-=(const int2048 &o) {
  return minus(o);
}

inline int2048 operator-(int2048 a, const int2048 &b) {
  return a.minus(b);
}

// Multiplication: Small & NTT
inline int2048 int2048::mul_small(const int2048 &x, const int2048 &y) {
  int2048 res;
  res.a.assign(x.a.size() + y.a.size(), 0);
  for (size_t i = 0; i < x.a.size(); i++) {
    long long carry = 0;
    for (size_t j = 0; j < y.a.size() || carry; j++) {
      long long cur = res.a[i + j] + carry + (long long)x.a[i] * (j < y.a.size() ? y.a[j] : 0);
      res.a[i + j] = (int)(cur % BASE);
      carry = cur / BASE;
    }
  }
  res.sign = x.sign * y.sign;
  res.trim();
  return res;
}

namespace ntt_ops {
  const int M1 = 998244353, M2 = 1004535809, M3 = 469762049;
  const long long M12 = 1002772198720536577LL;

  inline long long qpow_m(long long a, long long b, int mod) {
    long long res = 1;
    a %= mod;
    while (b > 0) {
      if (b & 1) res = (__int128)res * a % mod;
      a = (__int128)a * a % mod;
      b >>= 1;
    }
    return res;
  }

  inline void ntt_custom(std::vector<int> &a, int n, int opt, int mod, int g) {
    std::vector<int> rev(n);
    for (int i = 0; i < n; i++) {
      rev[i] = (rev[i >> 1] >> 1) | ((i & 1) ? (n >> 1) : 0);
      if (i < rev[i]) std::swap(a[i], a[rev[i]]);
    }
    for (int len = 2; len <= n; len <<= 1) {
      int m = len >> 1;
      long long wn = qpow_m(g, (mod - 1) / len, mod);
      if (opt == -1) wn = qpow_m(wn, mod - 2, mod);
      for (int i = 0; i < n; i += len) {
        long long w = 1;
        for (int j = 0; j < m; j++) {
          long long u = a[i + j];
          long long v = (long long)a[i + j + m] * w % mod;
          a[i + j] = (u + v) % mod;
          a[i + j + m] = (u - v + mod) % mod;
          w = w * wn % mod;
        }
      }
    }
    if (opt == -1) {
      long long inv = qpow_m(n, mod - 2, mod);
      for (int i = 0; i < n; i++) {
        a[i] = (long long)a[i] * inv % mod;
      }
    }
  }
}

inline int2048 int2048::mul_ntt(const int2048 &x, const int2048 &y) {
  const int SBASE = 1000;
  std::vector<int> a_split, b_split;
  a_split.reserve(x.a.size() * 3);
  b_split.reserve(y.a.size() * 3);
  for (int v : x.a) {
    a_split.push_back(v % SBASE);
    v /= SBASE;
    a_split.push_back(v % SBASE);
    v /= SBASE;
    a_split.push_back(v);
  }
  for (int v : y.a) {
    b_split.push_back(v % SBASE);
    v /= SBASE;
    b_split.push_back(v % SBASE);
    v /= SBASE;
    b_split.push_back(v);
  }
  while (a_split.size() > 1 && a_split.back() == 0) a_split.pop_back();
  while (b_split.size() > 1 && b_split.back() == 0) b_split.pop_back();

  int target_len = a_split.size() + b_split.size();
  int n = 1;
  while (n < target_len) n <<= 1;

  std::vector<int> a1(n, 0), a2(n, 0), a3(n, 0);
  std::vector<int> b1(n, 0), b2(n, 0), b3(n, 0);
  for (size_t i = 0; i < a_split.size(); i++) a1[i] = a2[i] = a3[i] = a_split[i];
  for (size_t i = 0; i < b_split.size(); i++) b1[i] = b2[i] = b3[i] = b_split[i];

  using namespace ntt_ops;
  ntt_custom(a1, n, 1, M1, 3);
  ntt_custom(b1, n, 1, M1, 3);
  for (int i = 0; i < n; i++) a1[i] = (long long)a1[i] * b1[i] % M1;
  ntt_custom(a1, n, -1, M1, 3);

  ntt_custom(a2, n, 1, M2, 3);
  ntt_custom(b2, n, 1, M2, 3);
  for (int i = 0; i < n; i++) a2[i] = (long long)a2[i] * b2[i] % M2;
  ntt_custom(a2, n, -1, M2, 3);

  ntt_custom(a3, n, 1, M3, 3);
  ntt_custom(b3, n, 1, M3, 3);
  for (int i = 0; i < n; i++) a3[i] = (long long)a3[i] * b3[i] % M3;
  ntt_custom(a3, n, -1, M3, 3);

  long long inv_M1_M2 = qpow_m(M1, M2 - 2, M2);
  long long inv_M12_M3 = qpow_m(M12 % M3, M3 - 2, M3);

  std::vector<long long> conv(target_len, 0);
  for (int i = 0; i < target_len; i++) {
    long long x1 = a1[i];
    long long x2 = a2[i];
    long long x3 = a3[i];

    long long t1 = ((x2 - x1) % M2 + M2) % M2;
    t1 = t1 * inv_M1_M2 % M2;
    long long x12 = x1 + t1 * M1;

    long long t2 = ((x3 - (x12 % M3)) % M3 + M3) % M3;
    t2 = t2 * inv_M12_M3 % M3;
    conv[i] = x12;
  }

  int2048 res;
  res.a.clear();
  __int128 carry = 0;
  for (size_t i = 0; i < conv.size() || carry; i += 3) {
    __int128 sum = carry;
    if (i < conv.size()) sum += conv[i];
    if (i + 1 < conv.size()) sum += (__int128)conv[i + 1] * 1000;
    if (i + 2 < conv.size()) sum += (__int128)conv[i + 2] * 1000000;
    res.a.push_back((int)(sum % BASE));
    carry = sum / BASE;
  }
  res.sign = x.sign * y.sign;
  res.trim();
  return res;
}

inline int2048 &int2048::operator*=(const int2048 &o) {
  *this = *this * o;
  return *this;
}

inline int2048 operator*(int2048 a, const int2048 &b) {
  if ((a.a.size() == 1 && a.a[0] == 0) || (b.a.size() == 1 && b.a[0] == 0)) {
    return int2048(0);
  }
  if (1LL * a.a.size() * b.a.size() <= 400) {
    return int2048::mul_small(a, b);
  } else {
    return int2048::mul_ntt(a, b);
  }
}

// Division Implementation
inline int2048 int2048::div_knuth(const int2048 &x, const int2048 &y, int2048 &r) {
  if (compare_abs(x, y) < 0) {
    r = x;
    return int2048(0);
  }
  if (y.a.size() == 1) {
    int divisor = y.a[0];
    int2048 q;
    q.a.resize(x.a.size());
    long long rem = 0;
    for (int i = (int)x.a.size() - 1; i >= 0; i--) {
      long long cur = rem * BASE + x.a[i];
      q.a[i] = (int)(cur / divisor);
      rem = cur % divisor;
    }
    q.trim();
    r = int2048(rem);
    return q;
  }

  int n = y.a.size();
  int m = x.a.size() - n;
  int d = BASE / (y.a.back() + 1);

  std::vector<long long> u(x.a.size() + 1, 0);
  long long carry = 0;
  for (size_t i = 0; i < x.a.size(); i++) {
    long long cur = (long long)x.a[i] * d + carry;
    u[i] = cur % BASE;
    carry = cur / BASE;
  }
  u[x.a.size()] = carry;

  std::vector<long long> v(y.a.size(), 0);
  carry = 0;
  for (size_t i = 0; i < y.a.size(); i++) {
    long long cur = (long long)y.a[i] * d + carry;
    v[i] = cur % BASE;
    carry = cur / BASE;
  }

  std::vector<int> q(m + 1, 0);
  long long v_top1 = v[n - 1];
  long long v_top2 = v[n - 2];

  for (int j = m; j >= 0; j--) {
    long long q_hat = (u[j + n] * BASE + u[j + n - 1]) / v_top1;
    long long r_hat = (u[j + n] * BASE + u[j + n - 1]) % v_top1;

    while (q_hat >= BASE || q_hat * v_top2 > r_hat * BASE + u[j + n - 2]) {
      q_hat--;
      r_hat += v_top1;
      if (r_hat >= BASE) break;
    }

    long long borrow = 0;
    for (int i = 0; i < n; i++) {
      long long p = q_hat * v[i] + borrow;
      long long sub = p % BASE;
      borrow = p / BASE;
      if (u[j + i] < sub) {
        u[j + i] += BASE - sub;
        borrow++;
      } else {
        u[j + i] -= sub;
      }
    }
    if (u[j + n] < borrow) {
      u[j + n] += BASE - borrow;
      q_hat--;
      long long c = 0;
      for (int i = 0; i < n; i++) {
        long long sum = u[j + i] + v[i] + c;
        u[j + i] = sum % BASE;
        c = sum / BASE;
      }
      u[j + n] = (u[j + n] + c) % BASE;
    } else {
      u[j + n] -= borrow;
    }
    q[j] = (int)q_hat;
  }

  int2048 res_q;
  res_q.a = q;
  res_q.trim();

  int2048 res_r;
  res_r.a.resize(n);
  long long rem = 0;
  for (int i = n - 1; i >= 0; i--) {
    long long cur = rem * BASE + u[i];
    res_r.a[i] = (int)(cur / d);
    rem = cur % d;
  }
  res_r.trim();
  r = res_r;
  return res_q;
}

inline void int2048::div_mod_abs(const int2048 &x, const int2048 &y, int2048 &q, int2048 &r) {
  q = div_knuth(x, y, r);
}

inline int2048 &int2048::operator/=(const int2048 &o) {
  *this = *this / o;
  return *this;
}

inline int2048 operator/(int2048 a, const int2048 &b) {
  int2048 q, r;
  int2048::div_mod_abs(a, b, q, r);

  if (a.sign == b.sign) {
    q.sign = 1;
    q.trim();
    return q;
  } else {
    if (r.a.size() == 1 && r.a[0] == 0) {
      q.sign = -1;
      q.trim();
      return q;
    } else {
      q = q + int2048(1);
      q.sign = -1;
      q.trim();
      return q;
    }
  }
}

inline int2048 &int2048::operator%=(const int2048 &o) {
  *this = *this % o;
  return *this;
}

inline int2048 operator%(int2048 a, const int2048 &b) {
  return a - (a / b) * b;
}

inline std::istream &operator>>(std::istream &is, int2048 &x) {
  std::string s;
  if (is >> s) {
    x.read(s);
  }
  return is;
}

inline std::ostream &operator<<(std::ostream &os, const int2048 &x) {
  if (x.sign == -1 && !(x.a.size() == 1 && x.a[0] == 0)) {
    os << '-';
  }
  os << (x.a.empty() ? 0 : x.a.back());
  char buf[16];
  for (int i = (int)x.a.size() - 2; i >= 0; i--) {
    snprintf(buf, sizeof(buf), "%09d", x.a[i]);
    os << buf;
  }
  return os;
}

inline bool operator==(const int2048 &x, const int2048 &y) {
  if (x.a.size() == 1 && x.a[0] == 0 && y.a.size() == 1 && y.a[0] == 0) return true;
  return x.sign == y.sign && x.a == y.a;
}

inline bool operator!=(const int2048 &x, const int2048 &y) {
  return !(x == y);
}

inline bool operator<(const int2048 &x, const int2048 &y) {
  if (x == y) return false;
  if (x.sign != y.sign) return x.sign < y.sign;
  int cmp = int2048::compare_abs(x, y);
  return x.sign == 1 ? cmp < 0 : cmp > 0;
}

inline bool operator>(const int2048 &x, const int2048 &y) {
  return y < x;
}

inline bool operator<=(const int2048 &x, const int2048 &y) {
  return !(y < x);
}

inline bool operator>=(const int2048 &x, const int2048 &y) {
  return !(x < y);
}

} // namespace sjtu

#endif
