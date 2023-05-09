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
};

template<class T>
int LowerBound(T val, T *array, int l, int r) {
  int ans = r + 1;
  while (l <= r) {
    int mid = (l + r) >> 1;
    if (val < array[mid]) {
      r = mid - 1, ans = mid;
    } else {
      l = mid + 1;
    }
  }
  return ans;
}
template<class T>
int UpperBound(T val, T *array, int l, int r) {
  int ans = 0;
  while (l <= r) {
    int mid = (l + r) >> 1;
    if (!(val < array[mid])) {
      l = mid + 1, ans = mid;
    } else {
      r = mid - 1;
    }
  }
  return ans;
}
template<class T>
int BinarySearch(T val, T *array, int l, int r) {
  int ans = r + 1;
  while (l <= r) {
    int mid = (l + r) >> 1;
    if (val.key < array[mid].key) {
      r = mid - 1;
    } else if (array[mid].key < val.key) {
      l = mid + 1;
    } else {
      r = mid - 1, ans = mid;
    }
  }
  return ans;
}