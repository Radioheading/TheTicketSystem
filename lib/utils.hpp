#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <cstring>
#include <iostream>
#include "../src/train.hpp"

template<int length>
class my_string {
 public:
  char info[length + 1];
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

// data_translating

int sum[15] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
int run_sum[15] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
const int month_days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int run_month_days[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
class Date {
  friend class Time;
 protected:
  int year;
  int month;
  int day;
  int current;
 public:
  Date() {
    year = 1900;
    month = 1;
    day = 1;
    current = 0;
    for (int i = 1; i < year; ++i) {
      if (i % 4 == 0 && i % 100 != 0 || i % 400 == 0) current += 366;
      else current += 365;
    }
    if ((year % 4 == 0 && year % 100 != 0 || year % 400 == 0)) current += run_sum[month - 1] + day;
    else current += sum[month - 1] + day;
  }
  void change(int _year, int _month, int _day) {
    year = _year, month = _month, day = _day;
    current = 0;
    for (int i = 1; i < year; ++i) {
      if (i % 4 == 0 && i % 100 != 0 || i % 400 == 0) current += 366;
      else current += 365;
    }
    if ((year % 4 == 0 && year % 100 != 0 || year % 400 == 0)) current += run_sum[month - 1] + day;
    else current += sum[month - 1] + day;
  }
  Date(int, int, int);
  inline friend int operator-(const Date &, const Date &);
  inline Date &operator=(const Date &);
  inline Date &operator+=(const int &);
  inline friend Date operator+(Date, const int &);
  inline friend Date operator-(Date, const int &);
  inline friend std::ostream &operator<<(std::ostream &, const Date &);
  inline friend bool operator<(const Date &, const Date &);
  friend bool operator!=(const Date & cmp_1, const Date &cmp_2) {
    return cmp_1.current != cmp_2.current;
  }
  Date &operator++() {
    int year_rest;
    current += 1;
    if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) {
      year_rest = run_sum[month - 1] + day;
      if (year_rest == 366) {
        year++;
        month = 1;
        day = 1;
      } else if (year_rest == run_sum[month]) {
        month++;
        day = 1;
      } else {
        day++;
      }
    } else {
      year_rest = sum[month - 1] + day;
      if (year_rest == 365) {
        year++;
        month = 1;
        day = 1;
      } else if (year_rest == sum[month]) {
        month++;
        day = 1;
      } else {
        day++;
      }
    }
    return *this;
  }
  Date &operator--() {
    int year_rest;
    current -= 1;
    if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) {
      year_rest = run_sum[month - 1] + day;
      if (year_rest == 1) {
        year--;
        month = 12;
        day = 31;
      } else if (day == 1) {
        month--;
        day = run_sum[month] - run_sum[month - 1];
      } else {
        day--;
      }
    } else {
      year_rest = sum[month - 1] + day;
      if (year_rest == 1) {
        year--;
        month = 12;
        day = 31;
      } else if (day == 1) {
        month--;
        day = sum[month - 1] - sum[month - 2];
      } else {
        day--;
      }
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
    return cmp_1.current <= cmp_2.current;
  }

  friend bool operator==(const Date &cmp_1, const Date &cmp_2) {
    return cmp_1.current == cmp_2.current;
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
Date::Date(int year_1, int month_1, int day_1) {
  current = 0;
  year = year_1;
  month = month_1;
  day = day_1;
  if (month < 1 || month > 12) {
    year = 1900;
    month = 1;
    day = 1;
  } else if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) {
    if (day < 1 || day > run_month_days[month]) {
      year = 1900;
      month = 1;
      day = 1;
    }
  } else if (!(year % 4 == 0 && year % 100 != 0 || year % 400 == 0)) {
    if (day < 1 || day > month_days[month]) {
      year = 1900;
      month = 1;
      day = 1;
    }
  }
  for (int i = 1; i < year; ++i) {
    if (i % 4 == 0 && i % 100 != 0 || i % 400 == 0) current += 366;
    else current += 365;
  }
  if ((year % 4 == 0 && year % 100 != 0 || year % 400 == 0)) current += run_sum[month - 1] + day;
  else current += sum[month - 1] + day;
}
bool operator<(const Date &cmp_1, const Date &cmp_2) {
  return cmp_1.current < cmp_2.current;
}
Date &Date::operator=(const Date &in) = default;
Date &Date::operator+=(const int &adder) {
  current += adder;
  int iter = 0;
  int sigma = 0;
  int year_mod, month_mod;
  while (true) {
    year_mod = current - sigma;
    ++iter;
    if (iter % 4 == 0 && iter % 100 != 0 || iter % 400 == 0) {
      sigma += 366;
    } else {
      sigma += 365;
    }
    if (current <= sigma) {
      break;
    }
  }
  year = iter;
  iter = 0;
  if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) {
    while (true) {
      month_mod = year_mod - run_sum[iter];
      ++iter;
      if (run_sum[iter] >= year_mod) break;
    }
  } else {
    while (true) {
      month_mod = year_mod - sum[iter];
      ++iter;
      if (sum[iter] >= year_mod) break;
    }
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
  stream_out << out.year << "-";
  stream_out << out.month << "-";
  stream_out << out.day;
  return stream_out;
}
int operator-(const Date &minus_1, const Date &minus_2) {
  return abs(minus_1.current + (-(minus_2.current)));
}

class Time {
 public:
  friend class train_system;
   int now;
   Date day;
 public:
  Time(Date _day = Date(), int _hour = 0, int _minute = 0) : day(_day), now(_hour * 60 + _minute) {}
  Time &operator+=(const int &adder) {
    now += adder;
    day += now / 1440, now %= 1440;
    return *this;
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
  friend bool leq_day(const Time &cmp_1, const Date &cmp_2) {
    return cmp_1.day < cmp_2 || cmp_1.day == cmp_2;
  }
  friend bool geq_day(const Time &cmp_1, const Date &cmp_2) {
    return cmp_2 < cmp_1.day || cmp_1.day == cmp_2;
  }
};

template <class iterator, class Compare = std::less<typename iterator::value_type>>
void sort(iterator left, iterator right, Compare cmp = Compare{}) {
  if (left - right >= -1) return;
  typename iterator::value_type index(*left);
  iterator l(left), r(--right);
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
#endif