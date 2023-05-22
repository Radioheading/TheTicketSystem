#ifndef BPT__CACHELIST_HPP_
#define BPT__CACHELIST_HPP_

#include <fstream>
#include <iostream>

using std::pair;
namespace Lee {
/*
 * @class CacheList
 * this is a simulation of the LRU policy, using a hashmap and a linklist to save the
 * recently-used block, which can boost efficiency (especially during find)
 */

template<class T>
class CachePool {
  static const int max_cache = 120 * 1024 / sizeof(T); // to satisfy cache size limit
  static const int map_size = 10140;
  static const int mod = 10133;
  constexpr static const int rehash[20] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
 private:
  struct cache_node {
    int address = 0;
    T data;
    cache_node *prev = nullptr, *next = nullptr;
    cache_node(int address_ = 0,
               cache_node *prev_ = nullptr,
               cache_node *next_ = nullptr)
        : address(address_), prev(prev_), next(next_) {}
  };
  class cache {
   private:
    std::fstream &out;
   public:
    int size = 0;
    cache_node *head, *tail;
    explicit cache(std::fstream &out_) : out(out_), size(0) {
      head = new cache_node(), tail = new cache_node();
      head->next = tail, tail->prev = head;
    }
    ~cache() {
      size = 0;
      cache_node *now = head->next;
      while (now != tail) {
        cache_node *temp = now;
        now = now->next;
        if (temp->data.changed && temp->address != 0) {
          temp->data.changed = false;
          out.seekp(temp->address);
          out.write(reinterpret_cast<char *>(&temp->data), sizeof(temp->data));
        }
        delete temp;
      }
      out.close();
      delete now, delete head;
    }

    void ClearPool() {
      size = 0;
      cache_node *now = head->next;
      while (now != tail) {
        cache_node *temp = now;
        now = now->next;
        delete temp;
      }
      out.close();
      head->next = tail, tail->prev = head;
    }

    void erase(cache_node *todo) {
      todo->prev->next = todo->next;
      todo->next->prev = todo->prev;
      delete todo;
      --size;
    }

    void pop_back() {
      cache_node *todo = tail->prev;
      if (todo->data.address == 0) return;
      if (todo->data.changed) {
        todo->data.changed = false;
        out.seekp(todo->address);
        out.write(reinterpret_cast<char *>(&todo->data), sizeof(todo->data));
      }
      todo->prev->next = tail;
      tail->prev = todo->prev;
      delete todo;
      --size;
    }

    std::pair<cache_node *, bool> push_front(const T &val) {
      auto *todo = new cache_node(val.address);
      todo->data = val;
      todo->prev = head;
      head->next->prev = todo;
      todo->next = head->next;
      head->next = todo;
      ++size;
      if (size == max_cache) {
        return std::make_pair(todo, false);
      }
      return std::make_pair(todo, true);
    }
  };
  cache block_pool;
  cache_node *storage[map_size] = {nullptr};
  pair<cache_node *, int> find(int id) {
    int pos = id % mod;
    int cnt = 0;
    while (pos < map_size) {
      if (storage[pos] && storage[pos]->address == id) {
        return pair<cache_node *, int>(storage[pos], pos);
      } else {
        pos += rehash[cnt];
        ++cnt;
      }
    }
    return pair<cache_node *, int>(nullptr, -1);
  }

  int find_valid(int id) {
    int pos = id % mod, cnt = 0;
    while (pos < map_size) {
      if (storage[pos]) {
        pos += rehash[cnt];
        ++cnt;
      } else {
        return pos;
      }
    }
    return -1;
  }

  int find_place(int id) {
    int pos = id % mod, cnt = 0;
    while (pos < map_size) {
      if (storage[pos] && storage[pos]->address == id) {
        return pos;
      } else {
        pos += rehash[cnt];
        ++cnt;
      }
    }
    return -1;
  }

  void erase_last() {
    cache_node *todo = block_pool.tail->prev;
    if (!todo) return;
    storage[find_place(todo->address)] = nullptr;
    block_pool.pop_back();
  }

  void Delete(cache_node *todo) {
    storage[find_place(todo->address)] = nullptr;
    block_pool.erase(todo);
  }
 public:
  CachePool(std::fstream &out_) : block_pool(out_) {}
  bool GetNode(T &todo, int id) {
    cache_node *search = find(id).first;
    if (search) {
      todo = search->data;
      Delete(search);
      return true;
    } else {
      return false;
    }
  }

  void InsertFront(const T &value) {
    auto res = block_pool.push_front(value);
    if (!res.second) {
      erase_last();
    }
    storage[find_valid(value.address)] = res.first;
  }

  void Erase(T &todo) {
    cache_node *temp = find(todo.address).first;
    Delete(temp);
  }

  void clear() {
    block_pool.ClearPool();
  }

  int size() {
    return block_pool.size;
  }
};
}
#endif //BPT__CACHELIST_HPP_