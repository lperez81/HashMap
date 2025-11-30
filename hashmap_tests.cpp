#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#include "hashmap.h"

using namespace std;
using namespace testing;

struct CollidingInt {
  int value;
  bool operator==(const CollidingInt& other) const {
    return value == other.value;
  }
};

namespace std {
template <>
struct hash<CollidingInt> {
  size_t operator()(const CollidingInt&) const noexcept {
    return 0;
  }
};
}  // namespace std

namespace {

class Random {
 private:
  static mt19937 rng;

 public:
  static void seed(int s) {
    Random::rng.seed(s);
  }

  /**
   * Generate a random integer in the range 0 (inclusive) to `max` (exclusive)
   */
  static int randInt(int max) {
    // Even though mt19937 is standardized, STL distributions aren't!
    // So, unfortunately, even though this is biased, we're forced to
    // do something like this. Technically uint32_fast_t isn't...
    // totally consistent across platforms? But within reason it works.
    return rng() % (max + 1);
  }
};

std::mt19937 Random::rng;

TEST(FakeTest, PleaseDeleteOnceYouWriteSome) {
  // If no tests exist, GoogleTest fails to compile with a fairly confusing
  // message. We have this empty test that does nothing to allow an empty
  // autograder submission to compile and regularly fail. Once you write your
  // own tests for the project, you can delete this one.
  EXPECT_THAT(1, Eq(1));
}

TEST(HashMapCore, DefaultConstructorEmptyAndCapacity) {
  HashMap<int, int> hm;
  EXPECT_TRUE(hm.empty());
  EXPECT_EQ(hm.size(), static_cast<size_t>(0));
  EXPECT_NE(hm.get_data(), nullptr);
  EXPECT_EQ(hm.get_capacity(), static_cast<size_t>(10));
}

TEST(HashMapCore, CustomCapacityConstructor) {
  HashMap<int, int> hm(20);
  EXPECT_TRUE(hm.empty());
  EXPECT_EQ(hm.size(), static_cast<size_t>(0));
  EXPECT_EQ(hm.get_capacity(), static_cast<size_t>(20));
}

TEST(HashMapCore, InsertSingleAndAtContains) {
  HashMap<int, string> hm;
  hm.insert(1, "one");

  EXPECT_FALSE(hm.empty());
  EXPECT_EQ(hm.size(), static_cast<size_t>(1));
  EXPECT_TRUE(hm.contains(1));
  EXPECT_EQ(hm.at(1), "one");
}

TEST(HashMapCore, InsertDuplicateDoesNotOverwriteAndSizeUnchanged) {
  HashMap<int, int> hm;
  hm.insert(5, 100);
  hm.insert(5, 200);  // should NOT overwrite or change size

  EXPECT_EQ(hm.size(), static_cast<size_t>(1));
  EXPECT_EQ(hm.at(5), 100);
}

TEST(HashMapCore, ContainsFalseForMissingKey) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  EXPECT_FALSE(hm.contains(2));
}

TEST(HashMapCore, AtThrowsForMissingKey) {
  HashMap<string, int> hm;
  hm.insert("hello", 42);
  EXPECT_THROW(hm.at("world"), out_of_range);
}

TEST(HashMapCore, ClearRemovesAllMappingsAndAllowsReuse) {
  HashMap<int, int> hm;
  for (int i = 0; i < 5; ++i) {
    hm.insert(i, i * 10);
  }
  ASSERT_EQ(hm.size(), static_cast<size_t>(5));
  EXPECT_FALSE(hm.empty());

  hm.clear();
  EXPECT_TRUE(hm.empty());
  EXPECT_EQ(hm.size(), static_cast<size_t>(0));

  for (int i = 0; i < 5; ++i) {
    EXPECT_FALSE(hm.contains(i));
    EXPECT_THROW(hm.at(i), out_of_range);
  }

  // Reuse after clear
  hm.insert(99, 123);
  EXPECT_EQ(hm.size(), static_cast<size_t>(1));
  EXPECT_TRUE(hm.contains(99));
  EXPECT_EQ(hm.at(99), 123);
}

TEST(HashMapCore, EraseExistingKeyReturnsValueAndRemovesMapping) {
  HashMap<int, string> hm;
  hm.insert(1, "one");
  hm.insert(2, "two");
  hm.insert(3, "three");

  string val = hm.erase(2);
  EXPECT_EQ(val, "two");
  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
  EXPECT_FALSE(hm.contains(2));
  EXPECT_THROW(hm.at(2), out_of_range);
}

TEST(HashMapCore, EraseThrowsOnMissingKey) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  EXPECT_THROW(hm.erase(2), out_of_range);
}

TEST(HashMapCore, CopyConstructorCopiesAllMappingsNonEmpty) {
  HashMap<int, int> hm;
  for (int i = 0; i < 20; ++i) {
    hm.insert(i, i * 2);
  }

  HashMap<int, int> copy(hm);

  EXPECT_EQ(copy.size(), hm.size());
  EXPECT_EQ(copy.get_capacity(), hm.get_capacity());

  for (int i = 0; i < 20; ++i) {
    EXPECT_TRUE(copy.contains(i));
    EXPECT_EQ(copy.at(i), hm.at(i));
  }

  // Modify original, copy should not change
  hm.insert(100, 200);
  EXPECT_FALSE(copy.contains(100));
}

TEST(HashMapCore, CopyConstructorWorksOnEmptyHashMap) {
  HashMap<int, int> hm;
  HashMap<int, int> copy(hm);

  EXPECT_TRUE(copy.empty());
  EXPECT_EQ(copy.size(), static_cast<size_t>(0));
}

TEST(HashMapCore, AssignmentOperatorCopiesMappingsAndCapacity) {
  HashMap<int, int> hm1;
  for (int i = 0; i < 10; ++i) {
    hm1.insert(i, i + 1);
  }

  HashMap<int, int> hm2;
  hm2.insert(100, 200);
  hm2.insert(200, 300);

  hm2 = hm1;

  EXPECT_EQ(hm2.size(), hm1.size());
  EXPECT_EQ(hm2.get_capacity(), hm1.get_capacity());

  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(hm2.contains(i));
    EXPECT_EQ(hm2.at(i), hm1.at(i));
  }

  // Ensure old keys are gone
  EXPECT_FALSE(hm2.contains(100));
  EXPECT_FALSE(hm2.contains(200));
}

TEST(HashMapCore, AssignmentOperatorHandlesEmptySource) {
  HashMap<int, int> hm1;
  hm1.insert(1, 10);
  hm1.insert(2, 20);

  HashMap<int, int> hm2;  // empty
  hm1 = hm2;

  EXPECT_TRUE(hm1.empty());
  EXPECT_EQ(hm1.size(), static_cast<size_t>(0));
}

TEST(HashMapCore, AssignmentOperatorSelfAssignmentSafe) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  hm.insert(2, 20);

  HashMap<int, int>& selfRef = hm;
  hm = selfRef;

  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
  EXPECT_TRUE(hm.contains(1));
  EXPECT_TRUE(hm.contains(2));
  EXPECT_EQ(hm.at(1), 10);
  EXPECT_EQ(hm.at(2), 20);
}

TEST(HashMapCore, ResizeOccursWhenLoadFactorExceedsThreshold) {
  HashMap<int, int> hm;  // default capacity 10
  size_t initialCap = hm.get_capacity();

  // Insert enough elements so that (size+1)/capacity > 1.5,
  // using int keys where hash(i) = i for predictability
  for (int i = 0; i < 16; ++i) {
    hm.insert(i, i);
  }

  size_t newCap = hm.get_capacity();
  EXPECT_GT(newCap, initialCap);  // should have resized
  EXPECT_EQ(newCap, initialCap * 2);

  // All keys should still be present with correct values
  for (int i = 0; i < 16; ++i) {
    EXPECT_TRUE(hm.contains(i));
    EXPECT_EQ(hm.at(i), i);
  }
}

// AUGMENTED TESTS (operator==, begin, next)

TEST(HashMapAugmented, OperatorEqualTrueForSameMappingsDifferentInsertOrder) {
  HashMap<int, int> hm1;
  HashMap<int, int> hm2;

  hm1.insert(1, 10);
  hm1.insert(2, 20);
  hm1.insert(3, 30);

  hm2.insert(3, 30);
  hm2.insert(1, 10);
  hm2.insert(2, 20);

  EXPECT_TRUE(hm1 == hm2);
  EXPECT_TRUE(hm2 == hm1);
}

TEST(HashMapAugmented, OperatorEqualFalseDifferentSize) {
  HashMap<int, int> hm1;
  HashMap<int, int> hm2;

  hm1.insert(1, 10);
  hm1.insert(2, 20);

  hm2.insert(1, 10);

  EXPECT_FALSE(hm1 == hm2);
  EXPECT_FALSE(hm2 == hm1);
}

TEST(HashMapAugmented, OperatorEqualFalseDifferentValuesForSameKey) {
  HashMap<int, int> hm1;
  HashMap<int, int> hm2;

  hm1.insert(1, 10);
  hm2.insert(1, 99);

  EXPECT_FALSE(hm1 == hm2);
}

TEST(HashMapAugmented, OperatorEqualEmptyMapsAreEqual) {
  HashMap<int, int> hm1;
  HashMap<int, int> hm2;
  EXPECT_TRUE(hm1 == hm2);
}

TEST(HashMapAugmented, BeginNextOnEmptyReturnsFalseImmediately) {
  HashMap<int, int> hm;
  hm.begin();
  int key;
  int value;
  EXPECT_FALSE(hm.next(key, value));
}

TEST(HashMapAugmented, BeginNextVisitAllElementsOnce) {
  HashMap<string, int> hm;
  hm.insert("apple", 1);
  hm.insert("banana", 2);
  hm.insert("cherry", 3);

  hm.begin();
  string k;
  int v;
  vector<pair<string, int>> seen;

  while (hm.next(k, v)) {
    seen.push_back({k, v});
  }

  EXPECT_EQ(seen.size(), hm.size());

  set<pair<string, int>> seenSet(seen.begin(), seen.end());
  set<pair<string, int>> expected = {
      {"apple", 1}, {"banana", 2}, {"cherry", 3}};

  EXPECT_EQ(seenSet, expected);
}

TEST(HashMapAugmented, BeginNextHandlesCollisionsInSameBucket) {
  HashMap<int, int> hm(10);  // capacity 10 so keys 0,10,20 collide

  hm.insert(0, 100);
  hm.insert(10, 200);
  hm.insert(20, 300);

  hm.begin();
  int k;
  int v;
  vector<pair<int, int>> seen;

  while (hm.next(k, v)) {
    seen.push_back({k, v});
  }

  EXPECT_EQ(seen.size(), hm.size());

  set<pair<int, int>> seenSet(seen.begin(), seen.end());
  set<pair<int, int>> expected = {{0, 100}, {10, 200}, {20, 300}};

  EXPECT_EQ(seenSet, expected);
}

TEST(HashMapAugmented, BeginNextAfterResizeStillVisitsAllElements) {
  HashMap<int, int> hm;

  // force resize
  for (int i = 0; i < 30; ++i) {
    hm.insert(i, i * 10);
  }

  hm.begin();
  int k;
  int v;
  set<pair<int, int>> seen;

  while (hm.next(k, v)) {
    seen.insert({k, v});
  }

  EXPECT_EQ(seen.size(), hm.size());

  for (int i = 0; i < 30; ++i) {
    EXPECT_TRUE(seen.count({i, i * 10}) == 1);
  }
}

TEST(HashMapErase, EraseDoesNotRemoveOtherCollidingKeys) {
  HashMap<CollidingInt, int> hm(5);

  CollidingInt k1{1}, k2{2}, k3{3};

  hm.insert(k1, 100);
  hm.insert(k2, 200);
  hm.insert(k3, 300);

  int removed = hm.erase(k2);
  EXPECT_EQ(removed, 200);

  EXPECT_TRUE(hm.contains(k1));
  EXPECT_TRUE(hm.contains(k3));
  EXPECT_EQ(hm.at(k1), 100);
  EXPECT_EQ(hm.at(k3), 300);
  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
}

TEST(HashMapCore, IntKeysEraseDoesNotRemoveOtherCollidingValues) {
  // Capacity 1: all int keys collide into the same bucket,
  HashMap<int, int> hm(1);

  hm.insert(1, 100);
  hm.insert(2, 200);
  hm.insert(3, 300);

  // Erase a middle key 
  int removed = hm.erase(2);
  EXPECT_EQ(removed, 200);

  // Other colliding keys MUST still be present and correct
  EXPECT_TRUE(hm.contains(1));
  EXPECT_TRUE(hm.contains(3));
  EXPECT_EQ(hm.at(1), 100);
  EXPECT_EQ(hm.at(3), 300);

  // Size should drop by exactly 1
  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
}

TEST(HashMapCore, EraseHeadOfCollisionChainKeepsRest) {
  HashMap<CollidingInt, int> hm(5);

  CollidingInt k1{1}, k2{2}, k3{3};

  // AFTER these inserts, k3 is HEAD.
  hm.insert(k1, 100);
  hm.insert(k2, 200);
  hm.insert(k3, 300);

  // Remove actual head k3
  int removed = hm.erase(k3);
  EXPECT_EQ(removed, 300);

  EXPECT_TRUE(hm.contains(k1));
  EXPECT_TRUE(hm.contains(k2));
  EXPECT_EQ(hm.at(k1), 100);
  EXPECT_EQ(hm.at(k2), 200);
  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
}

TEST(HashMapCore, EraseActualTrueHeadDoesNotRemoveOthers) {
  HashMap<CollidingInt, int> hm(5);

  CollidingInt k1{1}, k2{2}, k3{3};

  // Insert in reverse order so k1 becomes HEAD
  hm.insert(k3, 300);
  hm.insert(k2, 200);
  hm.insert(k1, 100);  // NEWEST, therefore the actual HEAD

  
  int removed = hm.erase(k1);
  EXPECT_EQ(removed, 100);

  EXPECT_TRUE(hm.contains(k2));
  EXPECT_TRUE(hm.contains(k3));
  EXPECT_EQ(hm.at(k2), 200);
  EXPECT_EQ(hm.at(k3), 300);

  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
}

TEST(HashMapRandomized, RandomInsertsAndLookupsWork) {
  HashMap<int, int> hm;
  Random::seed(123);

  vector<int> keys;
  for (int i = 0; i < 50; i++) {
    int k = Random::randInt(1000);
    keys.push_back(k);
    hm.insert(k, k + 1);
  }

  for (int k : keys) {
    if (hm.contains(k)) {
      EXPECT_EQ(hm.at(k), k + 1);
    }
  }
}

TEST(HashMapCore, EraseHeadOnlyRemovesHead) {
  HashMap<CollidingInt, int> hm(5);

  CollidingInt a{1}, b{2}, c{3};

  // Chain: HEAD -> c -> b -> a
  hm.insert(a, 100);
  hm.insert(b, 200);
  hm.insert(c, 300);

  int removed = hm.erase(c);  // erase TRUE head
  EXPECT_EQ(removed, 300);

  EXPECT_TRUE(hm.contains(a));
  EXPECT_TRUE(hm.contains(b));
  EXPECT_EQ(hm.at(a), 100);
  EXPECT_EQ(hm.at(b), 200);
  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
}

TEST(HashMapCore, EraseMiddleOnlyRemovesMiddle) {
  HashMap<CollidingInt, int> hm(5);

  CollidingInt a{1}, b{2}, c{3};

  hm.insert(a, 100);
  hm.insert(b, 200);
  hm.insert(c, 300);  // head

  int removed = hm.erase(b);  // middle
  EXPECT_EQ(removed, 200);

  EXPECT_TRUE(hm.contains(a));
  EXPECT_TRUE(hm.contains(c));
  EXPECT_EQ(hm.at(a), 100);
  EXPECT_EQ(hm.at(c), 300);
  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
}

TEST(HashMapCore, EraseTailOnlyRemovesTail) {
  HashMap<CollidingInt, int> hm(5);

  CollidingInt a{1}, b{2}, c{3};

  hm.insert(a, 100);  // tail
  hm.insert(b, 200);
  hm.insert(c, 300);  // head

  int removed = hm.erase(a);  // tail
  EXPECT_EQ(removed, 100);

  EXPECT_TRUE(hm.contains(b));
  EXPECT_TRUE(hm.contains(c));
  EXPECT_EQ(hm.at(b), 200);
  EXPECT_EQ(hm.at(c), 300);
  EXPECT_EQ(hm.size(), static_cast<size_t>(2));
}

TEST(HashMapCore, EraseMiddleWithSuccessorKeepsChainIntact) {
  HashMap<int, int> hm(1);

  // CollidingInt k1{1}, k2{2}, k3{3}, k4{4}, k5{5};

  //chain: HEAD -> k4 -> k3 -> k2 -> k1
  hm.insert(1, 100);
  hm.insert(2, 200);
  hm.insert(3, 300);
  hm.insert(4, 400);
  hm.insert(5, 500);
  hm.insert(6, 600);

  EXPECT_EQ(hm.size(), 6);
  // Erase k3 (a middle node with successor k2)
  int removed = hm.erase(3);
  EXPECT_EQ(removed, 300);
  EXPECT_FALSE(hm.contains(3));
  // Check all remaining nodes remain accessible
  EXPECT_TRUE(hm.contains(1));  // head
  EXPECT_TRUE(hm.contains(2));  // successor
  EXPECT_TRUE(hm.contains(4));  // tail
  EXPECT_TRUE(hm.contains(5));
  EXPECT_TRUE(hm.contains(6));

  EXPECT_EQ(hm.at(1), 100);
  EXPECT_EQ(hm.at(2), 200);
  EXPECT_EQ(hm.at(4), 400);
  EXPECT_EQ(hm.at(5), 500);
  EXPECT_EQ(hm.at(6), 600);

  EXPECT_EQ(hm.size(), 5);
}

}  // namespace
