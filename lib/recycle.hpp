#ifndef BPT__RECYCLE_HPP_
#define BPT__RECYCLE_HPP_
#include <string>
#include <fstream>

const int max_bin = 1e4 + 5;

struct before {
  int size = 0;
  int address[max_bin];
};
class bin {
 private:
  before store;
  std::fstream garbage;
 public:
  explicit bin(const std::string &bin_name) {
    garbage.open(bin_name);
    garbage.seekg(0, std::ios::beg);
    if (!garbage) {
      garbage.open(bin_name, std::ios::out);
      garbage.write(reinterpret_cast<char *>(&store), sizeof(store));
      garbage.close();
      garbage.open(bin_name);
    } else {
      garbage.read(reinterpret_cast<char *>(&store), sizeof(store));
    }
  }

  ~bin() {
    garbage.seekp(0);
    garbage.write(reinterpret_cast<char *>(&store), sizeof(store));
    garbage.close();
  }
  bool empty() const {
    return !store.size;
  }

  int pop_back() {
    // std::cout << store.address[store.size - 1] << "is getting out\n";
    return store.address[--store.size];
  }

  void push_back(const int &todo) {
    if (store.size == max_bin - 1) return;
    store.address[store.size++] = todo;
  }
};
#endif //BPT__RECYCLE_HPP_