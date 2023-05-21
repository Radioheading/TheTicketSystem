#ifndef MULTIBPT_HPP_
#define MULTIBPT_HPP_
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include "CacheList.hpp"
#include "recycle.hpp"
#include "vector.hpp"
#include "bpt.hpp"

template<class Key, class Key1, class T>
class MultiBPlusTree {
  struct Index;
  struct element;
  static const int max_size = 2048 / sizeof(element) * 2, min_size = max_size >> 1;
  static const int max_son = 2048 / sizeof(Index) * 2, min_son = max_son >> 1;
  enum NodeState { leaf, middle };
 private:
  bin tree_bin, data_bin;
  std::fstream tree, data;
  std::string tree_name, data_name;
  struct Index {
    Key key;
    Key1 key_1;

    inline friend bool operator<(const Index &cmp_1, const Index &cmp_2) {
      return cmp_1.key < cmp_2.key || cmp_1.key == cmp_2.key && cmp_1.key_1 < cmp_2.key_1;
    }

    inline friend bool operator==(const Index &cmp_1, const Index &cmp_2) {
      return cmp_1.key == cmp_2.key && cmp_1.key_1 == cmp_2.key_1;
    }
    explicit Index(const Key &_key = Key(), const Key1 &_key1 = Key1()) : key(_key), key_1(_key1) {}
  };
  struct element {
    Index index{};
    T value{};

    inline friend bool operator<(const element &cmp_1, const element &cmp_2) {
      return cmp_1.index < cmp_2.index;
    }

    inline friend bool operator==(const element &cmp_1, const element &cmp_2) {
      return cmp_1.index == cmp_2.index;
    }
    element &operator=(const element &obj) {
      if (&obj == this) return *this;
      index = obj.index;
      value = obj.value;
      return *this;
    }
    element() : index(Index()), value(T()) {}
    element(const Key &_key, const Key1 &_key1, const T &number) : index(_key, _key1), value(number) {}
  };
  struct node {
    int address = 0;
    bool changed = false;
    NodeState state = middle;
    int son_num = 0, son_pos[max_son];
    Index index[max_son - 1];
    node(bool did = false) : changed(did) {}
  } current_node;
  struct leaves {
    int address = 0;
    bool changed = false;
    int next_pos = 0, data_num = 0;
    element storage[max_size];
    leaves(bool did = false) : changed(did) {}
  } current_leaf;
  struct begin_tree {
    int start_place = sizeof(begin_tree);
    int end_place = sizeof(begin_tree) + sizeof(node);
  } tree_begin;
  struct begin_data {
    int end_place = sizeof(begin_data);
  } data_begin;
  const int node_size = sizeof(node);
  const int leaf_size = sizeof(leaves);
  CachePool<node> node_cache;
  CachePool<leaves> leaf_cache;
 public:
  friend class CachePool<node>;
  friend class CachePool<leaves>;
  node root;
  MultiBPlusTree(const std::string &_tree_name, const std::string &_data_name)
      : tree_name(_tree_name + ".db"),
        data_name(_data_name + ".db"),
        tree_bin(_tree_name + "'s garbage.db"),
        data_bin(_data_name + "'s garbage.db"),
        node_cache(tree),
        leaf_cache(data) {
    init();
  }
  ~MultiBPlusTree() {
    UpdateTree(), UpdateData();
    tree.seekp(root.address);
    tree.write(reinterpret_cast<char *>(&root), node_size);
  };

  sjtu::vector<T> find(const Key &key) {
    element another(key, Key1(), T());
    sjtu::vector<T> ret;
    current_node = root;
    while (current_node.state != leaf) {
      if (current_node.son_num == 0) {
        // std::cout << "no son!\n";
        return ret;
      }
      int place = LBound(another.index, current_node.index, 0, current_node.son_num - 2);
      ReadNode(current_node, current_node.son_pos[place]);
      WriteNode(current_node);
    }
    if (current_node.son_num == 0) {
      // std::cout << "no data!\n";
      return ret;
    }
    int search = LBound(another.index, current_node.index, 0, current_node.son_num - 2);
    ReadLeaf(current_leaf, current_node.son_pos[search]);
    int pos = LowerBound(another, current_leaf.storage, 0, current_leaf.data_num - 1);
    while (true) {
      for (int i = pos; i < current_leaf.data_num; ++i) {
        // std::cout << "checking: " << current_leaf.storage[i].index.key << ' ' << current_leaf.storage[i].index.key_1 << '\n';
        if (current_leaf.storage[i].index.key == key) {
          ret.push_back(current_leaf.storage[i].value);
        } else {
          WriteLeaves(current_leaf);
          return ret;
        }
      }
      WriteLeaves(current_leaf);
      if (current_leaf.next_pos) { // getting next leaf
        ReadLeaf(current_leaf, current_leaf.next_pos);
        pos = 0;
      } else break;
    }
    return ret;
  }

  T find_unique(const Key &key, const Key1 &key_1) {
    element another(key, key_1, T());
    current_node = root;
    while (current_node.state != leaf) {
      if (current_node.son_num == 0) {
        return another.value;
      }
      int place = LBound(another.index, current_node.index, 0, current_node.son_num - 2);
      ReadNode(current_node, current_node.son_pos[place]);
      WriteNode(current_node);
    }
    if (current_node.son_num == 0) {
      return another.value;
    }
    int search = LBound(another.index, current_node.index, 0, current_node.son_num - 2);
    ReadLeaf(current_leaf, current_node.son_pos[search]);
    int pos = LowerBound(another, current_leaf.storage, 0, current_leaf.data_num - 1);
    if (pos < current_leaf.data_num && current_leaf.storage[pos].index == another.index) {
      WriteLeaves(current_leaf);
      return current_leaf.storage[pos].value;
    } else {
      WriteLeaves(current_leaf);
      return another.value;
    }
  }

  void insert(const Key &key, const Key1 &key_1, const T &val) {
    element another(key, key_1, val);
    if (root.son_num == 0) { // nothing exist, first insert
      leaves first_leaf(false);
      first_leaf.address = data_begin.end_place;
      data_begin.end_place += leaf_size;
      first_leaf.data_num = 1, first_leaf.storage[0] = another;
      root.son_num = 1, root.son_pos[0] = first_leaf.address;
      data.seekp(first_leaf.address);
      data.write(reinterpret_cast<char *>(&first_leaf), sizeof(first_leaf));
      WriteLeaves(first_leaf);
      return;
    }
    if (!InternalInsert(root, another)) {// root splitting
      node new_root(false), vice_root(false);
      vice_root.state = root.state, new_root.state = middle;
      root.son_num = vice_root.son_num = min_son;
      for (int i = 1; i <= min_son; ++i) {
        vice_root.son_pos[i - 1] = root.son_pos[i + min_son - 1];
      }
      for (int i = 1; i < min_son; ++i) {
        vice_root.index[i - 1] = root.index[i + min_son - 1];
      }
      if (tree_bin.empty()) {
        vice_root.address = tree_begin.end_place, tree_begin.end_place += node_size;
      } else {
        vice_root.address = tree_bin.pop_back();
      }
      new_root.address = root.address;
      if (tree_bin.empty()) {
        root.address = tree_begin.end_place, tree_begin.end_place += node_size;
      } else {
        root.address = tree_bin.pop_back();
      }
      new_root.son_num = 2;
      new_root.index[0] = root.index[min_son - 1];
      new_root.son_pos[0] = root.address, new_root.son_pos[1] = vice_root.address;
      tree.seekp(vice_root.address);
      tree.write(reinterpret_cast<char *>(&vice_root), node_size);
      tree.seekp(root.address);
      tree.write(reinterpret_cast<char *>(&root), node_size);
      WriteNode(root), WriteNode(vice_root);
      root = new_root;
    }
  }

  void erase(const Key &key, const Key1 &key_1) {
    element another(key, key_1, T());
    bool checker = InternalErase(another, root);
    if (!checker && root.state == middle && root.son_num == 1) {
      // lowering the tree
      node new_root;
      ReadNode(new_root, root.son_pos[0]);
      tree_bin.push_back(new_root.address);
      new_root.address = root.address;
      root = new_root;
    }
  }

  void clear() {
    tree_bin.clear(), data_bin.clear();
    leaf_cache.clear(), node_cache.clear();
    init();
  }

 private:
  void init() {
    tree.open(tree_name), data.open(data_name);
    tree.seekg(0, std::ios::beg), data.seekg(0, std::ios::beg);
    if (!tree || !data) {
      tree.open(tree_name, std::ios::out), data.open(data_name, std::ios::out);
      tree.write(reinterpret_cast<char *>(&tree_begin), sizeof(tree_begin));
      data.write(reinterpret_cast<char *>(&data_begin), sizeof(data_begin));
      root.address = tree_begin.start_place, root.son_num = 0, root.state = leaf;
      tree.seekp(root.address);
      tree.write(reinterpret_cast<char *>(&root), node_size);
      tree.close(), data.close();
      tree.open(tree_name), data.open(data_name);
    } else {
      tree.read(reinterpret_cast<char *>(&tree_begin), sizeof(tree_begin));
      data.read(reinterpret_cast<char *>(&data_begin), sizeof(data_begin));
      tree.seekg(tree_begin.start_place);
      tree.read(reinterpret_cast<char *>(&root), sizeof(root));
    }
  }

  bool InternalInsert(node &todo, const element &another) {
    // false means its father ought to be modified
    int pos = LBound(another.index, todo.index, 0, todo.son_num - 2);
    if (todo.state == leaf) {
      leaves todo_leaf;
      ReadLeaf(todo_leaf, todo.son_pos[pos]);
      int search = LBound(another, todo_leaf.storage, 0, todo_leaf.data_num - 1);
      for (int i = todo_leaf.data_num + 1; i > search + 1; --i) {
        todo_leaf.storage[i - 1] = todo_leaf.storage[i - 2];
      }
      todo_leaf.storage[search] = another;
      ++todo_leaf.data_num, todo_leaf.changed = true;
      if (todo_leaf.data_num == max_size) {// block splitting
        // std::cout << "block splitting\n";
        leaves new_block(false);
        new_block.data_num = min_size, todo_leaf.data_num = min_size;
        for (int i = 1; i <= min_size; ++i) {
          new_block.storage[i - 1] = todo_leaf.storage[i + min_size - 1];
        }
        if (data_bin.empty()) {
          new_block.address = data_begin.end_place, data_begin.end_place += leaf_size;
        } else {
          new_block.address = data_bin.pop_back();
        }
        new_block.next_pos = todo_leaf.next_pos, todo_leaf.next_pos = new_block.address;
        data.seekp(new_block.address);
        data.write(reinterpret_cast<char *>(&new_block), leaf_size);
        WriteLeaves(todo_leaf), WriteLeaves(new_block);
        // updating the node
        ++todo.son_num;
        Index new_index = new_block.storage[0].index;
        int new_pos = new_block.address;
        for (int i = todo.son_num; i > pos + 2; --i) {
          todo.son_pos[i - 1] = todo.son_pos[i - 2];
        }
        for (int i = todo.son_num - 1; i > pos + 1; --i) {
          todo.index[i - 1] = todo.index[i - 2];
        }
        todo.son_pos[pos + 1] = new_pos, todo.index[pos] = new_index;
        todo.changed = true;
        if (todo.son_num == max_son) { // going up
          return false;
        } else {
          WriteNode(todo);
          return true;
        }
      } else {
        WriteNode(todo);
        WriteLeaves(todo_leaf);
        return true;
      }
    } else { // this is the node
      node todo_node;
      ReadNode(todo_node, todo.son_pos[pos]);
      if (InternalInsert(todo_node, another)) {
        WriteNode(todo);
        return true;
      } else { // needing to split
        todo_node.changed = true;
        node new_node(false);
        new_node.son_num = todo_node.son_num = min_son;
        for (int i = 1; i <= min_son; ++i) {
          new_node.son_pos[i - 1] = todo_node.son_pos[i + min_son - 1];
        }
        for (int i = 1; i < min_son; ++i) {
          new_node.index[i - 1] = todo_node.index[i + min_son - 1];
        }
        new_node.state = todo_node.state;
        if (tree_bin.empty()) {
          new_node.address = tree_begin.end_place, tree_begin.end_place += node_size;
        } else {
          new_node.address = tree_bin.pop_back();
        }
        tree.seekp(new_node.address);
        tree.write(reinterpret_cast<char *>(&new_node), node_size);
        WriteNode(todo_node), WriteNode(new_node);
        // updating todo
        Index new_index = todo_node.index[min_son - 1];
        int new_pos = new_node.address;
        for (int i = todo.son_num + 1; i > pos + 2; --i) {
          todo.son_pos[i - 1] = todo.son_pos[i - 2];
        }
        for (int i = todo.son_num; i > pos + 1; --i) {
          todo.index[i - 1] = todo.index[i - 2];
        }
        todo.son_pos[pos + 1] = new_pos, todo.index[pos] = new_index;
        ++todo.son_num, todo.changed = true;
        if (todo.son_num == max_son) { // going up
          return false;
        } else {
          WriteNode(todo);
          return true;
        }
      }
    }
  }

  bool InternalErase(const element &another, node &todo) {
    int pos = LBound(another.index, todo.index, 0, todo.son_num - 2);
    if (todo.state == leaf) {
      leaves todo_leaf;
      ReadLeaf(todo_leaf, todo.son_pos[pos]);
      int search = LowerBound(another, todo_leaf.storage, 0, todo_leaf.data_num - 1);
      if (search == todo_leaf.data_num || !(another == todo_leaf.storage[search])) {
        // not even deleting
        WriteNode(todo);
        WriteLeaves(todo_leaf);
        return true;
      }
      for (int i = search + 1; i < todo_leaf.data_num; ++i) {
        todo_leaf.storage[i - 1] = todo_leaf.storage[i];
      }
      --todo_leaf.data_num, todo_leaf.changed = true;
      if (todo_leaf.data_num < min_size) {
        // std::cout << "leaf adjusting" << '\n';
        // leaf adjusting
        todo.changed = true;
        leaves before, after;
        if (pos < todo.son_num - 1) { // borrowing behind
          ReadLeaf(after, todo.son_pos[pos + 1]);
          if (after.data_num > min_size) { // can borrow
            //std::cout << "borrowing behind\n";
            todo_leaf.storage[todo_leaf.data_num] = after.storage[0];
            ++todo_leaf.data_num;
            for (int i = 1; i < after.data_num; ++i) {
              after.storage[i - 1] = after.storage[i];
            }
            --after.data_num, after.changed = true;
            todo.index[pos] = after.storage[0].index;
            WriteNode(todo), WriteLeaves(todo_leaf), WriteLeaves(after);
            return true;
          }
        }
        if (pos > 0) { // borrowing front
          ReadLeaf(before, todo.son_pos[pos - 1]);
          if (before.data_num > min_size) {// can borrow
            // std::cout << "borrowing front\n";
            if (after.address) {
              WriteLeaves(after);
            }
            for (int i = todo_leaf.data_num + 1; i > 1; --i) {
              todo_leaf.storage[i - 1] = todo_leaf.storage[i - 2];
            }
            ++todo_leaf.data_num, todo_leaf.storage[0] = before.storage[before.data_num - 1];
            --before.data_num, before.changed = true;
            todo.index[pos - 1] = todo_leaf.storage[0].index;
            WriteNode(todo), WriteLeaves(todo_leaf), WriteLeaves(before);
            return true;
          }
        }
        if (pos < todo.son_num - 1) {
          if (before.next_pos) {
            WriteLeaves(before);
          }
          // merging the one behind
          // std::cout << "merging behind\n";
          for (int i = 1; i <= after.data_num; ++i) {
            todo_leaf.storage[todo_leaf.data_num + i - 1] = after.storage[i - 1];
          }
          todo_leaf.data_num += after.data_num, todo_leaf.next_pos = after.next_pos;
          WriteLeaves(todo_leaf), data_bin.push_back(after.address);
          for (int i = pos + 2; i < todo.son_num; ++i) {
            todo.son_pos[i - 1] = todo.son_pos[i];
          }
          for (int i = pos + 1; i < todo.son_num - 1; ++i) {
            todo.index[i - 1] = todo.index[i];
          }
          --todo.son_num;
          if (todo.son_num < min_son) {
            return false;
          } else {
            WriteNode(todo);
            return true;
          }
        }
        if (pos > 0) {
          if (after.next_pos) {
            WriteLeaves(after);
          }
          // merging the one at front
          // std::cout << "merging front\n";
          for (int i = 1; i <= todo_leaf.data_num; ++i) {
            before.storage[before.data_num + i - 1] = todo_leaf.storage[i - 1];
          }
          before.data_num += todo_leaf.data_num, before.next_pos = todo_leaf.next_pos;
          before.changed = true;
          WriteLeaves(before), data_bin.push_back(todo_leaf.address);
          for (int i = pos + 1; i < todo.son_num; ++i) {
            todo.son_pos[i - 1] = todo.son_pos[i];
          }
          for (int i = pos; i < todo.son_num - 1; ++i) {
            todo.index[i - 1] = todo.index[i];
          }
          --todo.son_num;
          if (todo.son_num < min_son) {
            return false;
          } else {
            WriteNode(todo);
            return true;
          }
        }
        // only son, can't do anything
        // std::cout << "only son\n";
        WriteNode(todo), WriteLeaves(todo_leaf);
        return true;
      } else {
        // need no adjustment
        WriteNode(todo), WriteLeaves(todo_leaf);
        return true;
      }
    } else {
      // std::cout << "adjusting node\n";
      node todo_node;
      ReadNode(todo_node, todo.son_pos[pos]);
      if (InternalErase(another, todo_node)) {
        WriteNode(todo);
        return true;
      } else {
        todo_node.changed = true, todo.changed = true;
        node before, after;
        // node adjusting
        if (pos < todo.son_num - 1) { // borrowing behind
          ReadNode(after, todo.son_pos[pos + 1]);
          if (after.son_num > min_son) { // can borrow
            // std::cout << "borrowing behind\n";
            todo_node.son_pos[todo_node.son_num] = after.son_pos[0];
            todo_node.index[todo_node.son_num - 1] = todo.index[pos], todo.index[pos] = after.index[0];
            ++todo_node.son_num;
            for (int i = 1; i < after.son_num; ++i) {
              after.son_pos[i - 1] = after.son_pos[i];
            }
            for (int i = 1; i < after.son_num - 1; ++i) {
              after.index[i - 1] = after.index[i];
            }
            --after.son_num, after.changed = true;
            WriteNode(todo), WriteNode(todo_node), WriteNode(after);
            return true;
          }
        }
        if (pos > 0) { // borrowing front
          ReadNode(before, todo.son_pos[pos - 1]);
          if (before.son_num > min_son) { // can borrow
            // std::cout << "borrowing front\n";
            if (after.address) {
              WriteNode(after);
            }
            for (int i = todo_node.son_num + 1; i > 1; --i) {
              todo_node.son_pos[i - 1] = todo_node.son_pos[i - 2];
            }
            for (int i = todo_node.son_num; i > 1; --i) {
              todo_node.index[i - 1] = todo_node.index[i - 2];
            }
            todo_node.son_pos[0] = before.son_pos[before.son_num - 1];
            todo_node.index[0] = todo.index[pos - 1];
            todo.index[pos - 1] = before.index[before.son_num - 2];
            ++todo_node.son_num;
            --before.son_num, before.changed = true;
            WriteNode(todo), WriteNode(todo_node), WriteNode(before);
            return true;
          }
        }
        if (pos < todo.son_num - 1) {
          if (before.address) {
            WriteNode(before);
          }
          // merging the one behind
          // std::cout << "merging behind\n";
          for (int i = 1; i <= after.son_num; ++i) {
            todo_node.son_pos[todo_node.son_num + i - 1] = after.son_pos[i - 1];
          }
          for (int i = 1; i < after.son_num; ++i) {
            todo_node.index[todo_node.son_num + i - 1] = after.index[i - 1];
          }
          todo_node.index[todo_node.son_num - 1] = todo.index[pos];
          todo_node.son_num += after.son_num;
          WriteNode(todo_node), tree_bin.push_back(after.address);
          for (int i = pos + 2; i < todo.son_num; ++i) {
            todo.son_pos[i - 1] = todo.son_pos[i];
          }
          for (int i = pos + 1; i < todo.son_num - 1; ++i) {
            todo.index[i - 1] = todo.index[i];
          }
          --todo.son_num;
          if (todo.son_num < min_son) {
            return false;
          } else {
            WriteNode(todo);
            return true;
          }
        }
        if (pos > 0) {
          if (after.address) {
            WriteNode(after);
          }
          // merging the one at front
          // std::cout << "merging front\n";
          for (int i = 1; i <= todo_node.son_num; ++i) {
            before.son_pos[before.son_num + i - 1] = todo_node.son_pos[i - 1];
          }
          for (int i = 1; i < todo_node.son_num; ++i) {
            before.index[before.son_num + i - 1] = todo_node.index[i - 1];
          }
          before.index[before.son_num - 1] = todo.index[pos - 1];
          before.son_num += todo_node.son_num;
          before.changed = true;
          WriteNode(before), tree_bin.push_back(todo_node.address);
          for (int i = pos + 1; i < todo.son_num; ++i) {
            todo.son_pos[i - 1] = todo.son_pos[i];
          }
          for (int i = pos; i < todo.son_num - 1; ++i) {
            todo.index[i - 1] = todo.index[i];
          }
          --todo.son_num;
          if (todo.son_num < min_son) {
            return false;
          } else {
            WriteNode(todo);
            return true;
          }
        }
      }
    }
    WriteNode(todo);
    return true;
  }
  void ReadNode(node &obj, int place) {
    if (!node_cache.GetNode(obj, place)) {
      tree.seekg(place);
      tree.read(reinterpret_cast<char *>(&obj), sizeof(obj));
    }
  }
  void ReadLeaf(leaves &obj, int place) {
    if (!leaf_cache.GetNode(obj, place)) {
      data.seekg(place);
      data.read(reinterpret_cast<char *>(&obj), sizeof(obj));
    }
  }
  void WriteNode(node &obj) {
    if (obj.address == 8) {// do not write root!
      return;
    }
    node_cache.InsertFront(obj);
  }
  void WriteLeaves(leaves &obj) {
    leaf_cache.InsertFront(obj);
  }
  void UpdateData() {
    data.seekp(0);
    data.write(reinterpret_cast<char *>(&data_begin), sizeof(data_begin));
  }
  void UpdateTree() {
    tree.seekp(0);
    tree.write(reinterpret_cast<char *>(&tree_begin), sizeof(tree_begin));
  }
};
#endif //BPT_HPP_

