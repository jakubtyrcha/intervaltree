#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <random>
#include <unordered_set>
#include "IntervalMultiTree.h"

using f32 = float;
using i32 = int;

struct Interval {
    f32 begin;
    f32 end;

    bool operator==(Interval other) const {
      return begin == other.begin && end == other.end;
    }

    bool Contains(f32 p) const {
        return begin <= p && p < end;
    }
};

// test adapter methods
template <typename T, typename V>
void TreeInsert(T &container, Interval interval, V && value) {
  container.Insert(interval, value);
}

template <typename T, typename V>
void TreeRemove(T &container, Interval interval, V && value) {
  container.Remove(interval);
}

template <typename V, typename T>
std::unordered_set<V> TreeQuery(T &container, f32 point) {
  std::vector<V> values;
  container.CollectQueryValues(point, values);
  return std::unordered_set<V>{values.begin(), values.end()};
}

//

TEST_CASE("Will look for value in the left subtree", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  TreeInsert(itree, Interval{0.f, 1.f}, 0);
  TreeInsert(itree, Interval{-0.5f, -0.25f}, -1);

  {
    auto queryResults = TreeQuery<i32>(itree, 0.f);
    REQUIRE(queryResults.contains(0));
    REQUIRE(!queryResults.contains(-1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, -0.5f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(queryResults.contains(-1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, -0.25f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(!queryResults.contains(-1));
  }
}

TEST_CASE("Will look for value in the right subtree", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  TreeInsert(itree, Interval{0.f, 1.f}, 0);
  TreeInsert(itree, Interval{-0.5f, -0.25f}, -1);
  TreeInsert(itree, Interval{0.5f, 2.f}, 1);

  {
    auto queryResults = TreeQuery<i32>(itree, 0.f);
    REQUIRE(queryResults.contains(0));
    REQUIRE(!queryResults.contains(-1));
    REQUIRE(!queryResults.contains(1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, 0.5f);
    REQUIRE(queryResults.contains(0));
    REQUIRE(!queryResults.contains(-1));
    REQUIRE(queryResults.contains(1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, 1.f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(!queryResults.contains(-1));
    REQUIRE(queryResults.contains(1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, 1.5f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(!queryResults.contains(-1));
    REQUIRE(queryResults.contains(1));
  }
}

TEST_CASE("Can query nested intervals", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  for(i32 i=0; i<20; i++) {
    TreeInsert(itree, Interval{static_cast<f32>(i), 20.f}, i);
  }

  for(i32 i=0; i<20; i++) {
    auto queryResults = TreeQuery<i32>(itree, static_cast<f32>(i));
    for(i32 j=0; j<=i; j++) {
      REQUIRE(queryResults.contains(j));
    }
    for(i32 j=i+1; j<20; j++) {
      REQUIRE(!queryResults.contains(j));
    }
  }
}

TEST_CASE("Can query disjoint intervals", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  for (i32 i = 0; i < 20; i++) {
    TreeInsert(itree, Interval{static_cast<f32>(i), static_cast<f32>(i + 1)},
               i);
  }

  for (i32 i = 0; i < 20; i++) {
    auto queryResults = TreeQuery<i32>(itree, static_cast<f32>(i));
    for (i32 j = 0; j < 20; j++) {
      if (i == j) {
        REQUIRE(queryResults.contains(j));
      } else {
        REQUIRE(!queryResults.contains(j));
      }
    }
  }
}

TEST_CASE("Can delete root and keep the tree valid", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  TreeInsert(itree, Interval{0.f, 1.f}, 0);
  TreeInsert(itree, Interval{1.f, 2.f}, 1);
  TreeInsert(itree, Interval{-1.f, 0.f}, -1);
  TreeRemove(itree, Interval{0.f, 1.f}, 0);
  {
    auto queryResults = TreeQuery<i32>(itree, 0.f);
    REQUIRE(!queryResults.contains(0));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, -1.f);
    REQUIRE(queryResults.contains(-1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, 1.f);
    REQUIRE(queryResults.contains(1));
  }
}