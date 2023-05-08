#ifndef BPT__EXCEPTIONS_HPP_
#define BPT__EXCEPTIONS_HPP_

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

class exception {
 protected:
  std::string message;
 public:
  exception() {}
  exception(const std::string &_message) : message(_message) {}
  virtual std::string what() {
    return message;
  }
};

class index_out_of_bound : public exception {
  /* __________________________ */
};

class runtime_error : public exception {
  /* __________________________ */
};

class invalid_iterator : public exception {
  /* __________________________ */
};

class container_is_empty : public exception {
  /* __________________________ */
};
}

#endif //BPT__EXCEPTIONS_HPP_
