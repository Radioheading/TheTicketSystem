/**
 * implement a container like std::map
 * using AVL Tree, eventually
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "exceptions.hpp"
#include <iostream>
using std::pair;
/**
 * this is a map implementation made by BruceLee, its paradigm is the AVL tree
 * references:《数据结构思想与实现第2版》, oi-wiki.org
 * (please view this with a critical mind, as they don't consider iterators,
 * making some obstacles during my implementation)
 * special thank to @qweryyfan,
 * he pointed out my problems and listened to my words and complaints
 * special thank to @conlesspan,
 * who rode a bicycle with me, talking everything and clearing up my irritation after debugging
 */

//manual of adjustment
//todo: check the validity of the iterator operations (fixed)
//todo: check the height problem (fixed)
//todo: check the delete problem (fixed)
//todo: check operator[](reference) (fixed, due to the absence of return in NodeInsert)

namespace Lee {
using namespace sjtu;
enum assign_check {
  yes, no
};
template<typename T>
struct my_type_traits{
  using iterator_assignable = typename T::iterator_assignable;
  static constexpr assign_check check = T::check;
};
//2.定义两个类，表示迭代器是否可被赋值的特性(这一步也可以使用枚举变量来实现)
struct my_true_type{};
struct my_false_type{};
template<typename U, typename T>
struct cmp {
  static constexpr bool value = false;
};
template<typename T>
struct cmp<T, T> {
  static constexpr bool value = true;
};
//3.分别在可被赋值的迭代器和不可被赋值的迭代器中定义 iterator_assignable 类型

template<
    class Key,
    class T,
    class Compare = std::less<Key>
>
class map {
 public:
  typedef pair<const Key, T> value_type;
 private:
  struct TreeNode {
    friend class map<Key, T, Compare>;
    TreeNode *ls, *rs, *father;
    value_type *datum;
    int height;
    /**
     * stated below are the basic functions of the Node
     */
    TreeNode(
        const value_type &_datum = NULL,
        TreeNode *_ls = nullptr,
        TreeNode *_rs = nullptr,
        TreeNode *_father = nullptr,
        int _height = 0)
        : ls(_ls), rs(_rs), father(_father), height(_height) {
      datum = (value_type *) malloc(sizeof(value_type));
      new(datum) value_type(_datum.first, _datum.second);
    };
    /**
     * please note that we still use malloc-free here as our constructor
     * because maybe Key doesn't have a default constructor
     */
    TreeNode(const TreeNode &obj) : ls(obj.ls), rs(obj.rs), father(obj.father), height(obj.height) {
      datum = (value_type *) malloc(sizeof(value_type));
      new(datum) value_type(obj.datum->first, obj.datum->second);
    }
    ~TreeNode() {
      datum->first.~Key(), datum->second.~T();
      free(datum);
      datum = nullptr;
    }
    friend bool operator<(const TreeNode &one, const TreeNode &another) {
      return Compare{}(one.datum->first, another.datum->first);
    }
  };

 private:
  int capacity;
  TreeNode *root;
  /**
   * listed below are the basic functions of the map
   * the internally-supplementary ones are listed as private
   */
  inline int GetHeight(const TreeNode *obj) const {
    return obj ? obj->height : 0;
  }

  inline void DeleteNode(TreeNode *&now) {
    if (!now) return;
    if (now->ls) DeleteNode(now->ls);
    if (now->rs) DeleteNode(now->rs);
    delete now, now = nullptr;
  }

  inline TreeNode *FindValue(TreeNode *now, const Key &key) const {
    if (!now) return nullptr;
    if (!Compare{}(key, now->datum->first) && !Compare{}(now->datum->first, key)) return now;
    if (Compare{}(key, now->datum->first)) return FindValue(now->ls, key);
    else return FindValue(now->rs, key);
  }

  inline void CopyNode(TreeNode *&one, const TreeNode *another) {
    if (!another) {
      one = nullptr;
      return;
    }
    one = new TreeNode(*another);
    one->father = another->father;
    if (another->ls) {
      CopyNode(one->ls, another->ls);
      one->ls->father = one;
    }
    if (another->rs) {
      CopyNode(one->rs, another->rs);
      one->rs->father = one;
    }
  }

  inline void LLSpin(TreeNode *&now) {
    TreeNode *after = now->ls;
    now->ls = after->rs;
    if (after->rs) {
      after->rs->father = now;
    }
    after->rs = now;
    after->father = now->father, now->father = after;
    // bottom-up updating
    now->height = std::max(GetHeight(now->ls), GetHeight(now->rs)) + 1;
    after->height = std::max(GetHeight(after->ls), GetHeight(after->rs)) + 1;
    now = after;
  }

  inline void RRSpin(TreeNode *&now) {
    TreeNode *after = now->rs;
    now->rs = after->ls;
    if (after->ls) {
      after->ls->father = now;
    }
    after->ls = now;
    after->father = now->father, now->father = after;
    // bottom-up updating
    now->height = std::max(GetHeight(now->ls), GetHeight(now->rs)) + 1;
    after->height = std::max(GetHeight(after->ls), GetHeight(after->rs)) + 1;
    now = after;
  }

  inline void LRSpin(TreeNode *&now) {
    RRSpin(now->ls);
    LLSpin(now);
  }

  inline void RLSpin(TreeNode *&now) {
    LLSpin(now->rs);
    RRSpin(now);
  }

  inline void maintain(TreeNode *&now, const value_type &x) {
    if (GetHeight(now->ls) - GetHeight(now->rs) < 2 && GetHeight(now->ls) - GetHeight(now->rs) > -2) return;
    if (GetHeight(now->ls) - GetHeight(now->rs) > 1) {
      if (Compare{}(x.first, now->ls->datum->first)) {
        LLSpin(now);
      } else {
        LRSpin(now);
      }
    } else {
      if (Compare{}(x.first, now->rs->datum->first)) {
        RLSpin(now);
      } else {
        RRSpin(now);
      }
    }
  }

  inline TreeNode *NodeInsert(TreeNode *&now, const value_type &x, TreeNode *its_father) {
    TreeNode *return_node;
    if (!now) {
      now = new TreeNode(x, nullptr, nullptr, its_father, 1);
      return now;
    } else {
      if (Compare{}(x.first, now->datum->first)) {
        return_node = NodeInsert(now->ls, x, now);
        maintain(now, x);
      } else {
        return_node = NodeInsert(now->rs, x, now);
        maintain(now, x);
      }
    }
    now->height = std::max(GetHeight(now->ls), GetHeight(now->rs)) + 1;
    return return_node;
  }

  inline bool EraseAdjust(TreeNode *&now, bool direction) {
    if (!direction) {
      // true means right
      if (GetHeight(now->rs) - GetHeight(now->ls) == 1) {
        // case a : now is balanced
        return true;
      }
      if (GetHeight(now->rs) == GetHeight(now->ls)) {
        // case b : now is balanced, but it is lowered, check its father!
        --now->height;
        return false;
      }
      // in the cases below, now isn't balanced
      if (GetHeight(now->rs->ls) < GetHeight(now->rs->rs)) {
        // case c : RRSpin, making now lower
        RRSpin(now);
        return false;
      }
      if (GetHeight(now->rs->ls) > GetHeight(now->rs->rs)) {
        // case d : RLSpin, making now lower
        RLSpin(now);
        return false;
      }
      if (GetHeight(now->rs->ls) == GetHeight(now->rs->rs)) {
        // case e : RRSpin(RLSpin also fits), making it balanced
        RRSpin(now);
        return true;
      }
    } else {
      if (GetHeight(now->ls) - GetHeight(now->rs) == 1) {
        return true;
      }
      if (GetHeight(now->ls) == GetHeight(now->rs)) {
        --now->height;
        return false;
      }
      if (GetHeight(now->ls->rs) < GetHeight(now->ls->ls)) {
        LLSpin(now);
        return false;
      }
      if (GetHeight(now->ls->rs) > GetHeight(now->ls->ls)) {
        LRSpin(now);
        return false;
      }
      if (GetHeight(now->ls->ls) == GetHeight(now->ls->rs)) {
        LLSpin(now);
        return true;
      }
    }
  }

  inline bool NodeErase(TreeNode *&now, const Key &x) {
    if (!now) return true;
    if (!Compare{}(x, now->datum->first) && !Compare{}(now->datum->first, x)) {
      if (now->ls && now->rs) {
        int action_case = 2;
        // having two sons, requiring replacement
        TreeNode *replace = now->rs;
        while (replace->ls) {
          action_case = 1;
          replace = replace->ls;
        }
        if (action_case == 1) {
          std::swap(replace->height, now->height);
          TreeNode *right_son = now->rs, *now_father = now->father;
          right_son->father = replace;
          if (replace->rs) {
            replace->rs->father = now;
          }
          if (now->ls) {
            now->ls->father = replace;
          }
          now->rs = replace->rs;
          replace->rs = right_son, replace->ls = now->ls;
          now->ls = nullptr;
          if (replace->father) {
            replace->father->ls = now;
          }
          now->father = replace->father, replace->father = now_father;
          now->datum->first.~Key(), now->datum->second.~T();
          new(now->datum) value_type(replace->datum->first, replace->datum->second);
          now = replace;

        }
        if (action_case == 2) {
          std::swap(now->height, replace->height);
          now->rs = replace->rs;
          if (replace->rs) {
            replace->rs->father = now;
          }
          if (now->ls) {
            now->ls->father = replace;
          }
          replace->rs = now, replace->ls = now->ls;
          now->ls = nullptr;
          replace->father = now->father, now->father = replace;
          now->datum->first.~Key(), now->datum->second.~T();
          new(now->datum) value_type(replace->datum->first, replace->datum->second);
          now = replace;
        }
        // now it manifests that we're deleting in the right subtree
        if (NodeErase(now->rs, now->datum->first)) {
          return true;
        }
        return EraseAdjust(now, true);
      } else {
        // having no/only one son
        TreeNode *before = now, *before_father = now->father;
        if (now->ls) {
          now = now->ls;
        } else {
          now = now->rs;
        }
        if (now) {
          now->father = before_father;
        }
        delete before;
        --capacity;
        return false;// this is because now is lowered
      }
    } else {
      if (Compare{}(x, now->datum->first)) {
        if (NodeErase(now->ls, x)) {
          return true;
        }
        return EraseAdjust(now, 0);
      } else {
        if (NodeErase(now->rs, x)) {
          return true;
        }
        return EraseAdjust(now, 1);
      }
    }
  }

  inline void Next(TreeNode *&now) const {
    if (!now) {
      return;
    }
    if (now->rs) {
      now = now->rs;
      while (now->ls) {
        now = now->ls;
      }
    } else {
      while (now->father && now->father->rs && now->father->rs == now) {
        now = now->father;
      }
      now = now->father;
    }
  }

  inline void Last(TreeNode *&now) const {
    if (!now) {
      return;
    }
    if (now->ls) {
      now = now->ls;
      while (now->rs) {
        now = now->rs;
      }
    } else {
      // puts("going up");
      while (now->father && now->father->ls && now->father->ls == now) {
        now = now->father;
      }
      now = now->father;
    }
  }

  inline TreeNode *First() const {
    TreeNode *now = root;
    if (!now) return nullptr;
    while (now->ls) now = now->ls;
    return now;
  }

  inline TreeNode *Back() const {
    TreeNode *now = root;
    if (!now) return nullptr;
    while (now->rs) now = now->rs;
    return now;
  }
 public:
  /**
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator {
   private:
    TreeNode *node;
    const map<Key, T, Compare> *from;
   public:
    // The following code is written for the C++ type_traits library.
    // Type traits is a C++ feature for describing certain properties of a type.
    // For instance, for an iterator, iterator::value_type is the type that the
    // iterator points to.
    // STL algorithms and containers may use these type_traits (e.g. the following
    // typedef) to work properly.
    // See these websites for more information:
    // https://en.cppreference.com/w/cpp/header/type_traits
    // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
    // About iterator_category: https://en.cppreference.com/w/cpp/iterator
    using difference_type = std::ptrdiff_t;
    using value_type = map::value_type;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::output_iterator_tag;
    friend class map;
    using iterator_assignable = my_true_type;
    static constexpr assign_check check = yes;
    // If you are interested in type_traits, toy_traits_test provides a place to
    // practice. But the method used in that test is old and rarely used, so you
    // may explore on your own.
    // Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
    // this part is only for bonus.


    iterator(TreeNode *_node = nullptr, const map *_from = nullptr) : node(_node), from(_from) {}
    iterator(const iterator &other) : node(other.node), from(other.from) {}
    iterator &operator=(const iterator &other) {
      if (&other == this) return *this;
      node = other.node, from = other.from;
      return *this;
    }

    iterator operator++(int) {
      if (!node) throw invalid_iterator();
      iterator stable_iter = *this;
      from->Next(node);
      return stable_iter;
    }

    iterator &operator++() {
      if (!node) throw invalid_iterator();
      from->Next(node);
      return *this;
    }

    iterator operator--(int) {
      if (!from->root || node == from->First()) throw invalid_iterator();
      iterator stable_iter = *this;
      if (!node) {
        // puts("end");
        node = from->Back();
      } else {
        from->Last(node);
      }
      return stable_iter;
    }

    iterator &operator--() {
      if (!from->root || node == from->First()) throw invalid_iterator();
      if (!node) {
        node = from->Back();
        return *this;
      } else {
        from->Last(node);
        return *this;
      }
    }

    value_type &operator*() const {
      if (!node) throw invalid_iterator();
      return *(node->datum);
    }

    bool operator==(const iterator &rhs) const {
      return (from == rhs.from && node == rhs.node);
    }

    bool operator==(const const_iterator &rhs) const {
      return (from == rhs.from && node == rhs.node);
    }

    bool operator!=(const iterator &rhs) const {
      return (from != rhs.from || node != rhs.node);
    }

    bool operator!=(const const_iterator &rhs) const {
      return (from != rhs.from || node != rhs.node);
    }

    value_type *operator->() const noexcept {
      if (!node) throw invalid_iterator();
      return node->datum;
    }
  };
  class const_iterator {
   private:
    TreeNode *node;
    const map<Key, T, Compare> *from;
   public:
    friend class map;
    using iterator_assignable = my_false_type;
    static constexpr assign_check check = no;
    const_iterator(TreeNode *_node = nullptr, const map *_from = nullptr) : node(_node), from(_from) {}
    const_iterator(const const_iterator &other) : node(other.node), from(other.from) {}
    const_iterator(const iterator &other) : node(other.node), from(other.from) {}
    const_iterator &operator=(const const_iterator &other) {
      if (&other == this) return *this;
      node = other.node, from = other.from;
      return *this;
    }
    const_iterator &operator=(const iterator &other) {
      node = other.node, from = other.from;
      return *this;
    }

    const_iterator operator++(int) {
      if (!from->root || !node) throw invalid_iterator();
      const_iterator stable_iter = *this;
      from->Next(node);
      return stable_iter;
    }

    const_iterator &operator++() {
      if (!from->root || !node) throw invalid_iterator();
      from->Next(node);
      return *this;
    }

    const_iterator operator--(int) {
      if (!from->root || node == from->First()) throw invalid_iterator();
      const_iterator stable_iter = *this;
      if (!node) {
        node = from->Back();
      } else {
        from->Last(node);
      }
      return stable_iter;
    }

    const_iterator &operator--() {
      if (!from->root || node == from->First()) throw invalid_iterator();
      if (!node) {
        node = from->Back();
        return *this;
      } else {
        from->Last(node);
        return *this;
      }
    }

    value_type &operator*() const {
      if (!node) throw invalid_iterator();
      return *(node->datum);
    }
    bool operator==(const const_iterator &rhs) const {
      return (from == rhs.from && node == rhs.node);
    }
    bool operator==(const iterator &rhs) const {
      return (from == rhs.from && node == rhs.node);
    }

    bool operator!=(const const_iterator &rhs) const {
      return (from != rhs.from || node != rhs.node);
    }

    bool operator!=(const iterator &rhs) const {
      return (from != rhs.from || node != rhs.node);
    }

    value_type *operator->() const noexcept {
      if (!node) throw invalid_iterator();
      return node->datum;
    }
  };

  map() : capacity(0), root(nullptr) {}

  map(const map &other) : capacity(other.capacity) {
    if (this == &other) return;
    CopyNode(root, other.root);
  }

  map &operator=(const map &other) {
    if (this == &other) return *this;
    DeleteNode(root), CopyNode(root, other.root);
    capacity = other.capacity;
    return *this;
  }

  ~map() {
    DeleteNode(root);
  }

  T &at(const Key &key) {
    TreeNode *exist = FindValue(root, key);
    if (!exist) throw index_out_of_bound();
    return exist->datum->second;
  }

  const T &at(const Key &key) const {
    TreeNode *exist = FindValue(root, key);
    if (!exist) throw index_out_of_bound();
    return exist->datum->second;
  }

  T &operator[](const Key &key) {
    T todo;
    TreeNode *found = FindValue(root, key);
    if (found) {
      return found->datum->second;
    } else {
      ++capacity;
      TreeNode *to_insert = NodeInsert(root, pair<Key, T>(key, todo), nullptr);
      return to_insert->datum->second;
    }
  }

  const T &operator[](const Key &key) const {
    TreeNode *exist = FindValue(root, key);
    if (!exist) throw index_out_of_bound();
    return exist->datum->second;
  }

  iterator begin() {
    return iterator(First(), this);
  }
  const_iterator cbegin() const {
    return const_iterator(First(), this);
  }

  iterator end() {
    return iterator(nullptr, this);
  }

  const_iterator cend() const {
    return const_iterator(nullptr, this);
  }

  bool empty() const {
    return (!capacity);
  }

  int size() const {
    return capacity;
  }

  void clear() {
    capacity = 0;
    DeleteNode(root);
  }

  pair<iterator, bool> insert(const value_type &value) {
    TreeNode *obj = FindValue(root, value.first);
    if (obj) {
      // insert fail
      return pair<iterator, bool>(iterator(obj, this), false);
    } else {
      ++capacity;
      return pair<iterator, bool>(iterator(NodeInsert(root, value, nullptr), this), true);
    }
  }

  void erase(iterator pos) {
    // the setting of bool is to check whether spinning is required
    if (pos.from != this || !pos.node) {
      throw 1;
    } else {
      NodeErase(root, pos.node->datum->first);
    }
  }

  int count(const Key &key) const {
    return (FindValue(root, key)) ? 1 : 0;
  }

  iterator find(const Key &key) {
    return iterator(FindValue(root, key), this);
  }

  const_iterator find(const Key &key) const {
    return const_iterator(FindValue(root, key), this);
  }
};
}
#endif