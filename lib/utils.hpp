#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <cstring>
#include <iostream>
#include <cmath>
#include "../src/train.hpp"

/*
 * ------------------------------class: my_string---------------------------------
 * namely a string with constant length, so that it can be written in binary files
 */
template<int length>
class my_string {
 public:
  char info[length + 1]{};
  my_string &operator=(const my_string &other) {
    if (&other == this) return *this;
    strcpy(info, other.info);
    return *this;
  }
  my_string &operator=(const std::string &other) {
    strcpy(info, other.c_str());
    return *this;
  }
  friend std::ostream &operator<<(std::ostream &out, const my_string &obj) {
    out << obj.info;
    return out;
  }
  my_string(const std::string &obj = "") {
    strcpy(info, obj.c_str());
  }
  friend bool operator<(const my_string &obj_1, const my_string &obj_2) {
    return strcmp(obj_1.info, obj_2.info) < 0;
  }
  friend bool operator==(const my_string &obj_1, const my_string &obj_2) {
    return strcmp(obj_1.info, obj_2.info) == 0;
  }
  explicit operator std::string() const {
    return std::string(info);
  }
};

/*
 * -----------------------------class: Date---------------------------------
 * namely a Date class enabling simple add, minus, comparing functions
 */
int sum[15] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
class Date {
  friend class Time;
 protected:
  int month;
  int day;
  int current;
 public:
  Date() {
    month = 1;
    day = 1;
    current = sum[month - 1] + day;
  }
  void change(int _month, int _day) {
    month = _month, day = _day;
    current = sum[month - 1] + day;
  }
  Date(int month_1, int day_1);
  inline friend int operator-(const Date &, const Date &);
  inline Date &operator=(const Date &);
  inline Date &operator+=(const int &);
  inline friend Date operator+(Date, const int &);
  inline friend Date operator-(Date, const int &);
  inline friend std::ostream &operator<<(std::ostream &, const Date &);
  inline friend bool operator<(const Date &, const Date &);
  friend bool operator!=(const Date &cmp_1, const Date &cmp_2) {
    return cmp_1.current != cmp_2.current;
  }
  Date &operator++() {
    current += 1;
    if (sum[month - 1] + day == sum[month]) {
      month++;
      day = 1;
    } else {
      day++;
    }
    return *this;
  }
  Date &operator--() {
    current -= 1;
    if (day == 1) {
      month--;
      day = sum[month - 1] - sum[month - 2];
    } else {
      day--;
    }
    return *this;
  }
  Date operator++(int) {
    Date return_Date = *this;
    ++*this;
    return return_Date;
  }
  Date operator--(int) {
    Date return_Date = *this;
    --*this;
    return return_Date;
  }

  friend bool operator<=(const Date &cmp_1, const Date &cmp_2) {
    return cmp_1.month < cmp_2.month || cmp_1.day <= cmp_2.day && cmp_1.month == cmp_2.month;
  }

  friend bool operator==(const Date &cmp_1, const Date &cmp_2) {
    return cmp_1.month == cmp_2.month && cmp_1.day == cmp_2.day;
  }

  explicit operator std::string() const {
    std::string month_s, day_s;
    if (month < 10) {
      month_s = '0' + std::to_string(month);
    } else {
      month_s = std::to_string(month);
    }
    if (day < 10) {
      day_s = '0' + std::to_string(day);
    } else {
      day_s = std::to_string(day);
    }
    return month_s + '-' + day_s;
  }
};
Date::Date(int month_1, int day_1) {
  month = month_1;
  day = day_1;
  current = sum[month - 1] + day;
}
bool operator<(const Date &cmp_1, const Date &cmp_2) {
  return cmp_1.current < cmp_2.current;
}
Date &Date::operator=(const Date &in) {
  if (&in == this) return *this;
  month = in.month, day = in.day, current = in.current;
  return *this;
}
Date &Date::operator+=(const int &adder) {
  current += adder;
  int year_mod = current, month_mod;
  int iter = 5;
  while (true) {
    month_mod = year_mod - sum[iter];
    ++iter;
    if (sum[iter] >= year_mod) break;
  }
  month = iter;
  day = month_mod;
  return (*this);
}
Date operator+(Date add_1, const int &add_2) {
  return (add_1 += add_2);
}
Date operator-(Date minus_1, const int &minus_2) {
  return (minus_1 += (-minus_2));
}
std::ostream &operator<<(std::ostream &stream_out, const Date &out) {
  stream_out << out.month << "-" << out.day;
  return stream_out;
}
int operator-(const Date &minus_1, const Date &minus_2) {
  return minus_1.current - minus_2.current;
}

/*
 * --------------------------class: Time-----------------------------
 * A Class with date and time during the day, also equipped with basic
 * add/minus/comparing functions
 */
class Time {
 public:
  int now; // the daytime (range from 0 to 1439)
  Date day;
 public:
  explicit Time(Date _day = Date(), int _hour = 0, int _minute = 0) : day(_day), now(_hour * 60 + _minute) {}
  Time &operator=(const Time &other) {
    if (&other == this) return *this;
    now = other.now;
    day = other.day;
    return *this;
  }
  Time &operator+=(const int &adder) {
    now += adder;
    day += now / 1440, now %= 1440;
    return *this;
  }
  friend int mod_minus(const Time &op1, const Time &op2) {
    return (op1.now - op2.now + 1440) % 1440;
  }
  /*
   * the so-called "operator-" returns in the form of integer
   */
  friend int operator-(const Time &op1, const Time &op2) {
    return 1440 * (op1.day - op2.day) + op1.now - op2.now;
  }
  void change(int _hour, int _minute) {
    now = _hour * 60 + _minute;
  }
  friend Time operator+(const Time &add_1, int add_2) {
    Time ans = add_1;
    ans.now += add_2;
    ans.day += ans.now / 1440, ans.now %= 1440;
    return ans;
  }
  friend bool operator!=(const Time &cmp_1, const Time &cmp_2) {
    return cmp_1.now != cmp_2.now || cmp_1.day != cmp_2.day;
  }
  explicit operator std::string() const {
    std::string ret;
    if (day.month < 10) ret += '0';
    ret += std::to_string(day.month) += '-';
    if (day.day < 10) ret += '0';
    ret += std::to_string(day.day) + ' ';
    if (now / 60 < 10) ret += '0';
    ret += std::to_string(now / 60) + ':';
    if (now % 60 < 10) ret += '0';
    ret += std::to_string(now % 60);
    return ret;
  }
  [[nodiscard]] std::string day_time() const {
    std::string ret;
    if (now / 60 < 10) ret += '0';
    ret += std::to_string(now / 60) + ':';
    if (now % 60 < 10) ret += '0';
    ret += std::to_string(now % 60);
    return ret;
  }

  friend bool operator<=(const Time &cmp_1, const Time &cmp_2) {
    return cmp_1.day < cmp_2.day || cmp_1.day == cmp_2.day && cmp_1.now <= cmp_2.now;
  }
  friend bool operator<(const Time &cmp_1, const Time &cmp_2) {
    return cmp_1.day < cmp_2.day || cmp_1.day == cmp_2.day && cmp_1.now < cmp_2.now;
  }
  friend bool leq_day(const Time &cmp_1, const Date &cmp_2) {
    return cmp_1.day <= cmp_2;
  }
  friend bool geq_day(const Time &cmp_1, const Date &cmp_2) {
    return cmp_2 <= cmp_1.day;
  }
  friend std::ostream &operator<<(std::ostream &stream_out, const Time &out) {
    stream_out << out.day << "-" << out.now / 60 << ' ' << out.now % 60;
    return stream_out;
  }
};

/*------------------------------function: sort------------------------------------
 * can sort a vector by using a method similar to std::sort
 * moreover, random strategy is deployed as conventional quick_sort may be O(n^2)
 */
template<class iterator, class Compare = std::less<typename iterator::value_type>>
void sort(iterator left, iterator right, Compare cmp = Compare{}) {
  if (left - right >= -1) return;
  iterator l(left), r(--right);
  std::swap(*l, *(l + rand() % (r - l)));
  typename iterator::value_type index(*left);
  ++right;
  while (r - l > 0) {
    while (cmp(*l, index) && right - l > 0 && r - l > 0) ++l;
    while (!cmp(*r, index) && *r != index && r - left >= 0 && r - l > 0) --r;
    if (r - l > 0) {
      typename iterator::value_type temp(*l);
      *l = *r;
      *r = temp;
    }
  }
  sort(left, r, cmp), sort(r + 1, right, cmp);
}

/*
 * ------------------------------function: MyHash------------------------------------------
 * A basic hash function to turn std::string/my_string to size_t, enabling faster comparing
 */
template<int size>
size_t MyHash(const my_string<size> &todo) {
  size_t ret = 0;
  for (int i = 0; i < size && todo.info[i]; ++i) {
    ret = ret * 263 + todo.info[i];
  }
  return ret;
}
size_t MyHash(const std::string &todo) {
  size_t ret = 0;
  for (int i = 0; i < todo.length() && todo[i]; ++i) {
    ret = ret * 263 + todo[i];
  }
  return ret;
}
#endif
