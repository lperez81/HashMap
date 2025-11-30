#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

template <typename KeyT, typename ValT>
class HashMap {
 private:
  struct ChainNode {
    const KeyT key;
    ValT value;
    ChainNode* next;

    ChainNode(KeyT key, ValT value) : key(key), value(value), next(nullptr) {
    }

    ChainNode(KeyT key, ValT value, ChainNode* next)
        : key(key), value(value), next(next) {
    }
  };

  ChainNode** data;
  size_t sz;
  size_t capacity;

  // Utility members for begin/next
  ChainNode* curr;
  size_t curr_idx;

  // Helper functions

  void initBuckets(size_t cap) {
    capacity = cap;
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }
  }

  void freeNodes() {
    if (!data) return;
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        ChainNode* nextNode = node->next;
        delete node;
        node = nextNode;
      }
      data[i] = nullptr;
    }
    sz = 0;
  }

  void rehash(size_t newCapacity) {
    if (newCapacity == 0) {
      newCapacity = 1;
    }

    ChainNode** newData = new ChainNode*[newCapacity];
    for (size_t i = 0; i < newCapacity; i++) {
      newData[i] = nullptr;
    }

    // Move existing nodes into new table, no new nodes created
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        ChainNode* nextNode = node->next;

        size_t idx = std::hash<KeyT>()(node->key) % newCapacity;

        // INSERT AT TAIL to preserve ordering
        if (newData[idx] == nullptr) {
          newData[idx] = node;
          node->next = nullptr;
        } else {
          ChainNode* t = newData[idx];
          while (t->next != nullptr) t = t->next;
          t->next = node;
          node->next = nullptr;
        }

        node = nextNode;
      }
      data[i] = nullptr;
    }

    delete[] data;
    data = newData;
    capacity = newCapacity;
  }

  // Central helper for computing the bucket index for a key
  size_t bucketIndex(const KeyT& key) const {
    return std::hash<KeyT>()(key) % capacity;
  }

 public:
  /**
   * Creates an empty `HashMap` with 10 buckets.
   */
  HashMap() {
    sz = 0;
    curr = nullptr;
    curr_idx = 0;
    initBuckets(10);
  }

  /**
   * Creates an empty `HashMap` with `capacity` buckets.
   */
  HashMap(size_t capacity) {
    sz = 0;
    curr = nullptr;
    curr_idx = 0;
    if (capacity == 0) {
      initBuckets(1);
    } else {
      initBuckets(capacity);
    }
  }

  /**
   * Checks if the `HashMap` is empty. Runs in O(1).
   */
  bool empty() const {
    return sz == 0;
  }

  /**
   * Returns the number of mappings in the `HashMap`. Runs in O(1).
   */
  size_t size() const {
    // TODO_STUDENT
    return sz;
  }

  /**
   * Adds the mapping `{key -> value}` to the `HashMap`. If the key already
   * exists, does not update the mapping (like the C++ STL map).
   *
   * Uses the hash value of the key to determine the location in the
   * underlying hash table. Creates exactly one new node; resizes by doubling
   * when the load factor exceeds 1.5.
   *
   * On resize, doesn't create new nodes, but rearranges existing ones.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  void insert(KeyT key, ValT value) {
    // TODO_STUDENT
    // Resize if the *resulting* load factor would exceed 1.5
    if (capacity == 0) {
      rehash(1);
    } else if (2 * (sz + 1) > 3 * capacity) {  // int-only check for > 1.5
      rehash(capacity * 2);
    }

    size_t idx = bucketIndex(key);  // use helper function
    ChainNode* node = data[idx];

    // If key already exists, do not update mapping
    while (node != nullptr) {
      if (node->key == key) {
        return;
      }
      node = node->next;
    }

    // Create exactly one new node and insert at head of chain
    ChainNode* newNode = new ChainNode(key, value, data[idx]);
    data[idx] = newNode;
    sz++;
  }

  /**
   * Return a reference to the value stored for `key` in the map.
   *
   * If key is not present in the map, throw `out_of_range` exception.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT& at(const KeyT& key) const {
    // TODO_STUDENT
    if (capacity == 0) {
      throw out_of_range("Key not found");
    }

    size_t idx = bucketIndex(key);
    ChainNode* node = data[idx];
    while (node != nullptr) {
      if (node->key == key) {
        return node->value;
      }
      node = node->next;
    }
    throw out_of_range("Key not found");
  }

  /**
   * Returns `true` if the key is present in the map, and false otherwise.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  bool contains(const KeyT& key) const {
    if (capacity == 0) {
      return false;
    }

    size_t idx = bucketIndex(key);
    ChainNode* node = data[idx];
    while (node != nullptr) {
      if (node->key == key) {
        return true;
      }
      node = node->next;
    }
    return false;
  }

  /**
   * Empties the `HashMap`, freeing all nodes. The bucket array may be left
   * alone.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  void clear() {
    // TODO_STUDENT
    freeNodes();
    curr = nullptr;  // don't forget to reset iterator state
    curr_idx = 0;
  }

  /**
   * Destructor, cleans up the `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  ~HashMap() {
    // TODO_STUDENT
    freeNodes();
    delete[] data;
    data = nullptr;
    capacity = 0;
    curr = nullptr;
    curr_idx = 0;
  }

  /**
   * Removes the mapping for the given key from the `HashMap`, and returns the
   * value.
   *
   * Throws `out_of_range` if the key is not present in the map. Creates no new
   * nodes, and does not update the key or value of any existing nodes.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT erase(const KeyT& key) {
    // TODO_STUDENT
    if (capacity == 0) {
      throw out_of_range("Key not found");
    }

    size_t idx = bucketIndex(key);
    ChainNode* node = data[idx];
    ChainNode* prev = nullptr;

    while (node != nullptr && !(node->key == key)) {
      prev = node;
      node = node->next;
    }

    if (node == nullptr) {
      throw out_of_range("Key not found");
    }

    // unlink node from chain
    if (prev == nullptr) {
      data[idx] = node->next;
    } else {
      prev->next = node->next;
    }

    ValT removedValue = node->value;
    delete node;
    sz--;
    return removedValue;
  }

  /**
   * Copy constructor.
   *
   * Copies the mappings from the provided `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings in `other`, and B is the
   * number of buckets.
   */
  HashMap(const HashMap& other) {
    // TODO_STUDENT
    sz = 0;
    curr = nullptr;
    curr_idx = 0;

    if (other.capacity == 0) {
      data = nullptr;
      capacity = 0;
      return;
    }

    initBuckets(other.capacity);

    for (size_t i = 0; i < other.capacity; i++) {
      ChainNode* otherNode = other.data[i];
      ChainNode** tailPtr = &data[i];
      while (otherNode != nullptr) {
        *tailPtr = new ChainNode(otherNode->key, otherNode->value);
        tailPtr = &((*tailPtr)->next);
        otherNode = otherNode->next;
        sz++;
      }
    }
  }

  /**
   * Assignment operator; `operator=`.
   *
   * Clears this table, and copies the mappings from the provided `HashMap`.
   *
   * Runs in O((N1+B1) + (N2+B2)), where N1 and B1 are the number of mappings
   * and buckets in `this`, and N2 and B2 are the number of mappings and buckets
   * in `other`.
   */
  HashMap& operator=(const HashMap& other) {
    // TODO_STUDENT
    if (this == &other) {
      return *this;
    }

    // Clean up existing data
    freeNodes();
    delete[] data;
    data = nullptr;
    capacity = 0;

    sz = 0;
    curr = nullptr;
    curr_idx = 0;

    if (other.capacity == 0) {
      return *this;
    }

    initBuckets(other.capacity);

    for (size_t i = 0; i < other.capacity; i++) {
      ChainNode* otherNode = other.data[i];
      ChainNode** tailPtr = &data[i];
      while (otherNode != nullptr) {
        *tailPtr = new ChainNode(otherNode->key, otherNode->value);
        tailPtr = &((*tailPtr)->next);
        otherNode = otherNode->next;
        sz++;
      }
    }

    return *this;
  }

  // =====================

  /**
   *
   * Checks if the contents of `this` and `other` are equivalent.
   *
   * Two `HashMap` objects are equivalent if they contain the same
   * mappings from key to value. To be equivalent, the two `HashMap`
   * objects need not have the elements saved in the same order
   * inside of the buckets.
   *
   * Runs in worst-case O(B*L) time, where B is the maximum number
   * of buckets in either of the `HashMap` objects and L is the length
   * of the  largest chain on any of the buckets.
   *
   */
  bool operator==(const HashMap& other) const {
    // TODO_STUDENT
    if (this == &other) {
      return true;
    }

    if (sz != other.sz) {
      return false;
    }

    // For every mapping in this, check it exists with same value in other
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        if (!other.contains(node->key)) {
          return false;
        }
        try {
          const ValT& otherVal = other.at(node->key);
          if (!(otherVal == node->value)) {
            return false;
          }
        } catch (const out_of_range&) {
          return false;
        }
        node = node->next;
      }
    }

    return true;
  }

  /**
   * Resets internal state for an iterative traversal.
   *
   * See `next` for usage details. Modifies nothing except for `curr` and
   * `curr_idx`.
   *
   * Runs in worst-case O(B), where B is the number of buckets.
   */
  void begin() {
    // TODO_STUDENT
    curr = nullptr;
    curr_idx = 0;

    if (capacity == 0) {
      return;
    }

    while (curr_idx < capacity && data[curr_idx] == nullptr) {
      curr_idx++;
    }

    if (curr_idx < capacity) {
      curr = data[curr_idx];
    } else {
      curr = nullptr;
    }
  }

  /**
   * Uses the internal state to return the "next" key and value
   * by reference, and advances the internal state. Returns `true` if the
   * reference parameters were set, and `false` otherwise.
   *
   * Example usage:
   *
   * ```c++
   * HashMap<string, int> hm;
   * hm.begin();
   * string key;
   * int value;
   * while (hm.next(key, val)) {
   *   cout << key << ": " << val << endl;
   * }
   * ```
   *
   * Does not visit the mappings in any defined order.
   *
   * Modifies nothing except for `curr` and `curr_idx`.
   *
   * Runs in worst-case O(B) where B is the number of buckets.
   */
  bool next(KeyT& key, ValT& value) {
    // TODO_STUDENT
    if (curr == nullptr) {
      return false;
    }

    // output current node
    key = curr->key;
    value = curr->value;

    // advance within chain if possible
    if (curr->next != nullptr) {
      curr = curr->next;
      return true;
    }

    // otherwise, advance to next non-empty bucket
    curr_idx++;
    while (curr_idx < capacity && data[curr_idx] == nullptr) {
      curr_idx++;
    }

    if (curr_idx < capacity) {
      curr = data[curr_idx];
    } else {
      curr = nullptr;
    }

    return true;
  }

  // ===============================================

  /**
   * Returns a pointer to the underlying memory managed by the `HashMap`.
   * For autograder testing purposes only.
   */
  void* get_data() const {
    return this->data;
  }

  /**
   * Returns the capacity of the underlying memory managed by the `HashMap`. For
   * autograder testing purposes only.
   */
  size_t get_capacity() {
    return this->capacity;
  }
};
