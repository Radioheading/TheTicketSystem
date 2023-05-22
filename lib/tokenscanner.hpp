#ifndef TOKENSCANNER_HPP_
#define TOKENSCANNER_HPP_

#include <string>
#include <cstring>

namespace Lee {
/*
 * ------------------------class: TokenScanner-------------------
 * it can divide a string based on the given separator,
 * its functions include HasMoreToken, NextToken, NextInteger and
 * GetChar, and you can reset the source string and separator by
 * function set
 */
class TokenScanner {
 private:
  std::string info;
  size_t length;
  char delim = ' ';
  int pos = 0;
 public:
  TokenScanner(const std::string &_info = "", char _delim = ' ')
      : info(_info), delim(_delim), pos(0), length(info.length()) {}
  ~TokenScanner() = default;

  void set(const std::string &_info = "", char _delim = ' ') {
    info = _info, delim = _delim, length = _info.length(), pos = 0;
  }

  std::string NextToken() {
    int l, r;
    while (info[pos] == delim && pos < length) {
      ++pos;
    }
    l = pos;
    while (info[pos] != delim && pos < length) {
      ++pos;
    }
    r = pos - 1;
    return info.substr(l, r - l + 1);
  }

  int NextInteger() {
    int ret = 0;
    while (info[pos] == delim) {
      ++pos;
    }
    while (info[pos] <= '9' && info[pos] >= '0' && pos < length) {
      ret = ret * 10 + (info[pos] - '0');
      ++pos;
    }
    return ret;
  }

  bool HasMoreToken() {
    if (pos == length) return false;
    while (info[pos] == delim) {
      ++pos;
      if (pos == length) return false;
    }
    return true;
  }

  char GetChar() {
    while (info[pos] == delim && pos < length) {
      ++pos;
    }
    return info[pos++];
  }
};
}
#endif