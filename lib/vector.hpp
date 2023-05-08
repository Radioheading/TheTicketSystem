#ifndef BPT__VECTOR_HPP_
#define BPT__VECTOR_HPP_
#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
 private:
  T *data;
  size_t capacity, current;
  /* -----------------------function : ChangeSize---------------------------
   * vector is, in essence, a dynamic array, which means we can't know
   * the real size of the array, thus dynamic size expansion ought to be
   * done.
   * According to the article found by @Polaris_Dane, the optimal size
   * expansion may be 1.68 * ans when it comes to memory saving. However,
   * the detailed proof was omitted
   * */
  void ChangeSize() {
    T *new_data = (T *) malloc((size_t) (2 * capacity) * sizeof(T));
    for (int i = 0; i < current; ++i) {
      // new_data[i] = data[i];
      /* warning: we haven't used constructor upon the allocated memory
       * (and the object may lack default constructor)
       * thus the method **placement new** can be employed to use copy constructor */
      new(new_data + i) T(data[i]);
      data[i].~T();
    }
    std::swap(data, new_data);
    free(new_data);
    capacity = (size_t) (2 * capacity);
  }
 public:
  /**
   * TODO
   * a type for actions of the elements of a vector, and you should write
   *   a class named const_iterator with same interfaces.
   */
  /**
   * you can see RandomAccessIterator at CppReference for help.
   */
  class const_iterator;
  class iterator {
    // The following code is written for the C++ type_traits library.
    // Type traits is a C++ feature for describing certain properties of a type.
    // For instance, for an iterator, iterator::value_type is the type that the
    // iterator points to.
    // STL algorithms and containers may use these type_traits (e.g. the following
    // typedef) to work properly. In particular, without the following code,
    // @code{std::sort(iter, iter1);} would not compile.
    // See these websites for more information:
    // https://en.cppreference.com/w/cpp/header/type_traits
    // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
    // About iterator_category: https://en.cppreference.com/w/cpp/iterator
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;
    friend class vector;
   private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */
    size_t position;
    vector *from;
   public:
    /*
     * constructors: default ones with given parameters
     */
    iterator(int _position = 0, vector *_from = nullptr) : position(_position), from(_from) {}
    iterator(const iterator &other) : position(other.position), from(other.from) {};
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */

    iterator operator+(const int &n) const {
      return iterator(position + n, from);
    }
    iterator operator-(const int &n) const {
      return iterator(position - n, from);
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw invalid_iterator.
    int operator-(const iterator &rhs) const {
      if (from != rhs.from) throw invalid_iterator();
      return (difference_type) (position - rhs.position);
    }
    iterator &operator+=(const int &n) {
      position += n;
      return *this;
    }
    iterator &operator-=(const int &n) {
      position -= n;
      return *this;
    }
    /**
     * TODO iter++
     */
    iterator operator++(int) {
      iterator stable_iter(position, from);
      ++position;
      return stable_iter;
    }
    /**
     * TODO ++iter
     */
    iterator &operator++() {
      ++position;
      return *this;
    }
    /**
     * TODO iter--
     */
    iterator operator--(int) {
      iterator stable_iter(position, from);
      --position;
      return stable_iter;
    }
    /**
     * TODO --iter
     */
    iterator &operator--() {
      --position;
      return *this;
    }
    /**
     * TODO *it
     */
    T &operator*() const {
      return from->data[position];
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same memory address).
     */
    bool operator==(const iterator &rhs) const {
      return (rhs.from == from && position == rhs.position);
    }
    bool operator==(const const_iterator &rhs) const {
      return (rhs.from == from && position == rhs.position);
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return (rhs.from != from || position != rhs.position);
    }
    bool operator!=(const const_iterator &rhs) const {
      return (rhs.from != from || position != rhs.position);
    }
  };
  /**
   * TODO
   * has same function as iterator, just for a const object.
   */
  class const_iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;
    friend class vector;
   private:
    size_t position;
    const vector *from;
   public:
    // the member functions and the constructors
    const_iterator(int _position = 0, const vector *_from = nullptr) : position(_position), from(_from) {};
    const_iterator(const const_iterator &other) : position(other.position), from(other.from) {};
    const_iterator operator+(const int &n) const {
      return const_iterator(position + n, from);
    }
    const_iterator operator-(const int &n) const {
      return const_iterator(position - n, from);
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw invalid_iterator.
    int operator-(const const_iterator &rhs) const {
      if (from != rhs.from) throw invalid_iterator();
      return (difference_type) (position - rhs.position);
    }
    const_iterator &operator+=(const int &n) {
      position += n;
      return *this;
    }
    const_iterator &operator-=(const int &n) {
      position -= n;
      return *this;
    }
    /**
     * TODO iter++
     */
    const_iterator operator++(int) {
      const_iterator stable_iter(position, from);
      ++position;
      return stable_iter;
    }
    /**
     * TODO ++iter
     */
    const_iterator operator++() {
      ++position;
      return *this;
    }
    /**
     * TODO iter--
     */
    const_iterator operator--(int) {
      const_iterator stable_iter(position, from);
      --position;
      return stable_iter;
    }
    /**
     * TODO --iter
     */
    const_iterator &operator--() {
      --position;
      return *this;
    }
    /**
     * TODO *it
     */
    const T &operator*() const {
      return from->data[position];
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same memory address).
     */
    bool operator==(const iterator &rhs) const {
      return (rhs.from == from && rhs.position == position);
    }
    bool operator==(const const_iterator &rhs) const {
      return (rhs.from == from && rhs.position == position);
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return (rhs.from != from || rhs.position != position);
    }
    bool operator!=(const const_iterator &rhs) const {
      return (rhs.from != from || rhs.position != position);
    }
  };
  /**
   * TODO Constructs
   * At least two: default constructor, copy constructor
   */
  vector() : data(nullptr), capacity(2), current(0) {
    data = (T *) malloc(capacity * sizeof(T));
  }
  vector(const vector &other) : capacity(other.capacity), current(other.current) {
    data = (T *) malloc(capacity * sizeof(T));
    for (int i = 0; i < current; ++i) {
      //data[i] = other[i];
      new(data + i) T(other[i]);
    }
  }
  /**
   * TODO Destructor
   */
  ~vector() {
    for (int i = 0; i < current; ++i) {
      data[i].~T();
    }
    free(data);
  }
  /**
   * TODO Assignment operator
   */
  vector &operator=(const vector &other) {
    if (this == &other) return *this;
    for (int i = 0; i < current; ++i) {
      data[i].~T();
    }
    free(data);
    capacity = other.capacity, current = other.current;
    data = (T *) malloc(other.capacity * sizeof(T));
    for (int i = 0; i < current; ++i) {
      new(data + i) T(other[i]);
    }
    return *this;
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   */
  T &at(const size_t &pos) {
    if (pos < 0 || pos >= current) throw index_out_of_bound();
    return data[pos];
  }
  const T &at(const size_t &pos) const {
    if (pos < 0 || pos >= current) throw index_out_of_bound();
    return data[pos];
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   * !!! Pay attentions
   *   In STL this operator does not check the boundary but I want you to do.
   */
  T &operator[](const size_t &pos) {
    if (pos < 0 || pos >= current) throw index_out_of_bound();
    return data[pos];
  }
  const T &operator[](const size_t &pos) const {
    if (pos < 0 || pos >= current) throw index_out_of_bound();
    return data[pos];
  }
  /**
   * access the first element.
   * throw container_is_empty if size == 0
   */
  const T &front() const {
    if (current == 0) throw container_is_empty();
    return data[0];
  }
  /**
   * access the last element.
   * throw container_is_empty if size == 0
   */
  const T &back() const {
    if (current == 0) throw container_is_empty();
    return data[current - 1];
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() {
    return iterator(0, this);
  }
  const_iterator cbegin() const {
    return const_iterator(0, this);
  }
  /**
   * returns an iterator to the end.
   */
  iterator end() {
    return iterator(current, this);
  }
  const_iterator cend() const {
    return const_iterator(current, this);
  }
  /**
   * checks whether the container is empty
   */
  bool empty() const {
    return (current == 0);
  }
  /**
   * returns the number of elements
   */
  size_t size() const {
    return current;
  }
  /**
   * clears the contents
   */
  void clear() {
    for (int i = 0; i < current; ++i) {
      data[i].~T();
    }
    current = 0;
  }
  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {
    if (current + 1 == capacity) ChangeSize();
    new(data + current) T(data[current - 1]);
    for (int i = current - 1; i >= (int) pos.position; --i) {
      data[i + 1] = data[i];
    }
    data[pos.position] = value;
    ++current;
    return iterator(pos.position, this);
  }
  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {
    if (ind > size()) throw index_out_of_bound();
    iterator insert_iter(ind, this);
    if (current + 1 == capacity) ChangeSize();
    new(data + current) T(data[current - 1]);
    for (int i = current - 1; i >= (int) ind; --i) {
      data[i + 1] = data[i];
    }
    data[ind] = value;
    ++current;
    return insert_iter;
  }
  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is returned.
   */
  iterator erase(iterator pos) {
    data[pos.position].~T();
    for (int i = pos.position; i < current - 1; ++i) {
      data[i] = data[i + 1];
    }
    --current;
    return iterator(pos.position, this);
  }
  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {
    if (ind >= size()) throw index_out_of_bound();
    data[ind].~T();
    for (int i = ind; i < current - 1; ++i) {
      data[i] = data[i + 1];
    }
    --current;
    return iterator(ind, this);
  }
  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {
    if (current + 1 == capacity) ChangeSize();
    // data[current] = value;
    new(data + current) T(value);
    ++current;
  }
  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {
    if (current == 0) throw container_is_empty();
    --current;
    data[current].~T();
  }
};
}

#endif //BPT__VECTOR_HPP_
