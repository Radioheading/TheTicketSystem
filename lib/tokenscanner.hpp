#ifndef TOKENSCANNER_HPP_
#define TOKENSCANNER_HPP_

#include <string>
#include <cstring>

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
    std::string ret;
    while (info[pos] == delim && pos < length) {
      ++pos;
    }
    while (info[pos] != delim && pos < length) {
      ret += info[pos];
      ++pos;
    }
    return info;
  }

  int NextInteger() {
    int ret = 0;
    while (info[pos] == delim) {
      ++pos;
    }
    while (info[pos] != delim && pos < length) {
      ret = ret * 10 + info[pos] - '0';
      ++pos;
    }
    return ret;
  }

  bool HasMoreToken() {
    while (info[pos] == delim) {
      ++pos;
      if (pos == length) return false;
    }
    return true;
  }

  char GetChar() {
    return info[pos++];
  }
};

#endif