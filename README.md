# HashMap
hashmap.h               # HashMap class interface and implementation
hashmap_main.cpp        # Driver program for running the HashMap
hashmap_tests.cpp       # Unit tests for HashMap behavior and edge cases
Makefile                # Build rules for compiling and testing
.clang-format           # Code formatting configuration
suppr.txt               # Suppression file (for memory/debug tooling)

It was built as part of **CS 251 – Data Structures** at the University of Illinois Chicago.This project is a from-scratch implementation of a HashMap in C++ designed for efficient key-value storage and retrieval. It includes collision handling, custom hashing logic, and extensive test cases written to verify correctness and performance. The project emphasizes memory management, debugging, and understanding how associative data structures work under the hood.
## Features

- Array-based bucket storage
- Custom hash function wrapper
- Collision handling (e.g., chaining with linked lists or similar structure)
- `insert`, `erase`, `contains`, and `at`/lookup operations
- Automatic size tracking
- Self-written test cases to validate behavior and edge cases

---

## Step-by-Step Explanation of the Project

### 1. Designing the Data Structure

- Decided to implement a **hash table** that maps keys to values using:
  - An internal **array/vector of buckets**
  - Each bucket storing **one or more key–value pairs** (to handle collisions)
- Chose a representation for each entry, such as:
  - `struct Entry { Key key; Value value; /* next pointer if chaining */ };`
- Chose a strategy for **collision resolution** (e.g., separate chaining).

---

### 2. Implementing the Core Storage

- Created an internal container, e.g. `std::vector<Bucket>` or similar.
- Chose an initial **capacity** (number of buckets) and stored it as a member variable.
- Wrote a helper function to:
  - Hash the key using `std::hash<Key>` (or a custom functor).
  - Map the hash value into a bucket index: `index = hash(key) % capacity`.

---

### 3. Handling Insertions

- Implemented an `insert(const Key& key, const Value& value)` function that:
  - Computes the bucket index for the given key.
  - Checks if the key already exists in that bucket.
    - If it exists, does **not** overwrite the existing value (depending on spec) and returns a status.
    - If it does not exist, **adds a new entry** to that bucket.
  - Updates the `size` of the HashMap only when a new key is inserted.
- This step reinforced:
  - How collisions are handled.
  - How to search within a bucket efficiently.

---

### 4. Implementing Lookup and Contains

- Implemented a `contains(const Key& key)` function that:
  - Computes the bucket index.
  - Scans that bucket for the given key.
  - Returns `true` if found, `false` otherwise.
- Implemented an `at(const Key& key)` (or similar) that:
  - Locates the key in the bucket.
  - Returns a **reference** to the stored value.
  - Throws an exception or handles the error if the key is not found.
- These functions helped practice:
  - Safe access patterns.
  - Returning references instead of copies when appropriate.

---

### 5. Erasing Elements

- Implemented an `erase(const Key& key)` function that:
  - Computes the bucket index.
  - Finds the entry within the bucket and removes it.
  - Decrements the `size` if the key was present.
- Paid special attention to:
  - Not accidentally erasing other entries when collisions occurred.
  - Keeping buckets in a valid state after removal (fixing links or indices).

---

### 6. Managing Capacity and Performance

- Considered how **load factor** (size / capacity) affects performance.
- (Optional) Implemented **rehashing** to grow the table when it becomes too full:
  - Allocated a new bucket array with a larger capacity.
  - Reinserted all existing key–value pairs into the new table using the new capacity.
- This step showed how resizing can keep operations efficient even as more data is added.

---

### 7. Writing Test Cases

- Created **custom test cases** to validate:
  - Basic operations: `insert`, `contains`, `erase`, `at`.
  - Behavior with duplicate keys.
  - Correct handling of collisions.
  - Edge cases (empty map, erasing missing keys, etc.).
- Used these tests to:
  - Catch logic errors.
  - Verify that changes didn’t break existing behavior.
  - Build confidence in the implementation.

---

### 8. Debugging and Refinement

- Used print statements and/or a debugger to trace:
  - Which bucket a key was mapped to.
  - How entries moved during rehashing or erase.
- Fixed issues related to:
  - Incorrect bucket indexing.
  - Size not updating correctly.
  - Collisions accidentally overwriting other entries.
- Refactored code for readability and maintainability once core logic was stable.

---

## How to Build and Run

```bash
# Example (adjust filenames as needed)
g++ -std=c++17 -Wall -Wextra -O2 -o hashmap_test main.cpp HashMap.cpp
./hashmap_test
