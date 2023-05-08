#include <string>
#include <cstring>
#include <iostream>

template <int length>
class my_string {
 public:
  char info[length + 1];
  my_string &operator=(const my_string &other) {
    strcpy(info, other.info);
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
  std::string operator() {

  }
};