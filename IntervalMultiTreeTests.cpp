#include "IntervalMultiTree.h"
#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <random>
#include <unordered_set>
#include <memory>

using f32 = float;
using i32 = int;

struct Interval {
  f32 begin;
  f32 end;

  bool operator==(Interval other) const {
    return begin == other.begin && end == other.end;
  }

  bool Contains(f32 p) const { return begin <= p && p < end; }
};

struct IntInterval {
  i32 begin;
  i32 end;

  bool operator==(Interval other) const {
    return begin == other.begin && end == other.end;
  }

  bool Contains(i32 p) const { return begin <= p && p < end; }
};

// test adapter methods
template <typename T, typename I, typename V>
void TreeInsert(T &container, I interval, V &&value) {
  container.Insert(interval, value);
}

template <typename T, typename I, typename V>
void TreeRemove(T &container, I interval, V &&value) {
  container.Remove(interval, value);
}

template <typename V, typename T, typename E>
std::unordered_set<V> TreeQuery(T &container, E point) {
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

TEST_CASE("Right rotate updates the node range", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  TreeInsert(itree, Interval{2.f, 3.f}, 0);
  TreeInsert(itree, Interval{1.f, 2.f}, 1);
  TreeInsert(itree, Interval{0.f, 1.f}, 2);
  REQUIRE(TreeQuery<i32>(itree, 2.5f).contains(0));
}

TEST_CASE("Can query nested intervals", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  for (i32 i = 0; i < 20; i++) {
    TreeInsert(itree, Interval{static_cast<f32>(i), 20.f}, i);
  }

  for (i32 i = 0; i < 20; i++) {
    auto queryResults = TreeQuery<i32>(itree, static_cast<f32>(i));
    for (i32 j = 0; j <= i; j++) {
      REQUIRE(queryResults.contains(j));
    }
    for (i32 j = i + 1; j < 20; j++) {
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

TEST_CASE("Can insert multiple intervals with shared begin", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  TreeInsert(itree, Interval{0.f, 1.f}, 0);
  TreeInsert(itree, Interval{0.f, 2.f}, 1);

  {
    auto queryResults = TreeQuery<i32>(itree, 2.f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(!queryResults.contains(1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, -1.f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(!queryResults.contains(1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, 0.f);
    REQUIRE(queryResults.contains(0));
    REQUIRE(queryResults.contains(1));
  }
  {
    auto queryResults = TreeQuery<i32>(itree, 1.f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(queryResults.contains(1));
  }
}

TEST_CASE("Can insert multiple values and remove a single value", "[interval_multitree]") {
  IntervalMultiTree<Interval, i32> itree;
  TreeInsert(itree, Interval{0.f, 1.f}, 0);
  TreeInsert(itree, Interval{0.f, 1.f}, 1);

  {
    auto queryResults = TreeQuery<i32>(itree, 0.f);
    REQUIRE(queryResults.contains(0));
    REQUIRE(queryResults.contains(1));
  }

  TreeRemove(itree, Interval{0.f, 1.f}, 0);
  {
    auto queryResults = TreeQuery<i32>(itree, 0.f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(queryResults.contains(1));
  }

  TreeRemove(itree, Interval{0.f, 1.f}, 1);
  {
    auto queryResults = TreeQuery<i32>(itree, 0.f);
    REQUIRE(!queryResults.contains(0));
    REQUIRE(!queryResults.contains(1));
  }
}

TEST_CASE("Interval tree works with shared pointers", "[interval_multitree]") {
  auto tracked = std::make_shared<int>(0);
  auto tracked1 = std::make_shared<int>(0);
  {
    IntervalMultiTree<Interval, std::shared_ptr<int>> itree;
    TreeInsert(itree, Interval{0.f, 1.f}, tracked);
    TreeInsert(itree, Interval{0.f, 1.f}, tracked1);
    TreeInsert(itree, Interval{0.f, 1.f}, std::make_shared<int>(0));
    TreeInsert(itree, Interval{0.f, 1.f}, std::make_shared<int>(1));
    REQUIRE(tracked.use_count() == 2);
    REQUIRE(tracked1.use_count() == 2);
    TreeRemove(itree, Interval{0.f, 1.f}, tracked);
    REQUIRE(tracked.use_count() == 1);
    REQUIRE(tracked1.use_count() == 2);
  }
  REQUIRE(tracked.use_count() == 1);
  REQUIRE(tracked1.use_count() == 1);
}

TEST_CASE("Can remove from a subtree with left and right children preserving value count",
          "[interval_multitree]") {
  IntervalMultiTree<IntInterval, std::shared_ptr<int>> itree;
  std::vector<std::shared_ptr<int>> pool;
  auto MakeValue = [&pool](int x) {
    auto v = std::make_shared<int>(x);
    pool.push_back(v);
    return v;
  };
  TreeInsert(itree, IntInterval{1, 2}, MakeValue(1));
  TreeInsert(itree, IntInterval{0, 1}, MakeValue(0));
  TreeInsert(itree, IntInterval{3, 4}, MakeValue(3));
  TreeInsert(itree, IntInterval{2, 3}, MakeValue(2));
  TreeInsert(itree, IntInterval{4, 5}, MakeValue(4));

  for (auto &v : pool) {
    REQUIRE(v.use_count() == 2);
  }

  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 1).contains(pool[0]));
  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 0).contains(pool[1]));
  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 3).contains(pool[2]));
  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 2).contains(pool[3]));
  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 4).contains(pool[4]));

  TreeRemove(itree, IntInterval{3, 4}, pool[2]);

  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 1).contains(pool[0]));
  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 0).contains(pool[1]));
  REQUIRE(!TreeQuery<std::shared_ptr<int>>(itree, 3).contains(pool[2]));
  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 2).contains(pool[3]));
  REQUIRE(TreeQuery<std::shared_ptr<int>>(itree, 4).contains(pool[4]));

  REQUIRE(pool[0].use_count() == 2);
  REQUIRE(pool[1].use_count() == 2);
  REQUIRE(pool[2].use_count() == 1);
  REQUIRE(pool[3].use_count() == 2);
  REQUIRE(pool[4].use_count() == 2);

  itree.Clear();
  for (auto &v : pool) {
    REQUIRE(v.use_count() == 1);
  }
}

TEST_CASE("Removing missing interval is a noop",
          "[interval_multitree]") {

  IntervalMultiTree<IntInterval, int> itree;
  TreeInsert(itree, IntInterval{0, 1}, 0);
  TreeInsert(itree, IntInterval{1, 2}, 1);
  TreeRemove(itree, IntInterval{0, 2}, 0);
  TreeRemove(itree, IntInterval{0, 2}, 1);
  REQUIRE(TreeQuery<int>(itree, 0).contains(0));
  REQUIRE(TreeQuery<int>(itree, 1).contains(1));
}

TEST_CASE("Removing missing value is a noop",
          "[interval_multitree]") {

  IntervalMultiTree<IntInterval, int> itree;
  TreeInsert(itree, IntInterval{0, 1}, 0);
  TreeInsert(itree, IntInterval{1, 2}, 1);
  TreeRemove(itree, IntInterval{0, 1}, 1);
  TreeRemove(itree, IntInterval{1, 2}, 0);
  REQUIRE(TreeQuery<int>(itree, 0).contains(0));
  REQUIRE(TreeQuery<int>(itree, 1).contains(1));
}

TEST_CASE("Timeline test", "[interval_multitree]") {
  IntervalMultiTree<IntInterval, int> itree;
  TreeInsert(itree, IntInterval{0, 10}, 0);
  TreeInsert(itree, IntInterval{10, 20}, 1);
  for (i32 i = 0; i < 20; i++) {
    TreeInsert(itree, IntInterval{i, i + 1}, i + 2);
  }
  {
    auto query0 = TreeQuery<int>(itree, 0);
    REQUIRE(query0.contains(0));
    REQUIRE(query0.contains(2));
    REQUIRE(!query0.contains(1));
    REQUIRE(!query0.contains(3));
  }
  for (i32 i = 1; i < 10; i++) {
    auto queryi = TreeQuery<int>(itree, i);
    REQUIRE(queryi.contains(0));
    REQUIRE(!queryi.contains(1));
    REQUIRE(!queryi.contains(i + 1));
    REQUIRE(queryi.contains(i + 2));
    REQUIRE(!queryi.contains(i + 3));
  }
  for (i32 i = 10; i < 20; i++) {
    auto queryi = TreeQuery<int>(itree, i);
    REQUIRE(!queryi.contains(0));
    REQUIRE(queryi.contains(1));
    REQUIRE(!queryi.contains(i + 1));
    REQUIRE(queryi.contains(i + 2));
    REQUIRE(!queryi.contains(i + 3));
  }
  {
    auto query20 = TreeQuery<int>(itree, 20);
    REQUIRE(!query20.contains(0));
    REQUIRE(!query20.contains(1));
    REQUIRE(!query20.contains(2));
    REQUIRE(!query20.contains(3));
    REQUIRE(!query20.contains(22));
  }
  TreeRemove(itree, IntInterval{10, 20}, 1);
  for (i32 i = 0; i < 20; i += 2) {
    TreeRemove(itree, IntInterval{i, i + 1}, i + 2);
  }
  {
    auto query0 = TreeQuery<int>(itree, 0);
    REQUIRE(query0.contains(0));
    REQUIRE(!query0.contains(2));
    REQUIRE(!query0.contains(1));
    REQUIRE(!query0.contains(3));
  }
  for (i32 i = 1; i < 10; i++) {
    auto queryi = TreeQuery<int>(itree, i);
    REQUIRE(queryi.contains(0));
    REQUIRE(!queryi.contains(1));
    REQUIRE(!queryi.contains(i + 1));
    if ((i % 2) != 0) {
      REQUIRE(queryi.contains(i + 2));
    }
    REQUIRE(!queryi.contains(i + 3));
  }
  for (i32 i = 10; i < 20; i++) {
    auto queryi = TreeQuery<int>(itree, i);
    REQUIRE(!queryi.contains(0));
    REQUIRE(!queryi.contains(1));
    REQUIRE(!queryi.contains(i + 1));
    if ((i % 2) != 0) {
      REQUIRE(queryi.contains(i + 2));
    }
    REQUIRE(!queryi.contains(i + 3));
  }
  {
    auto query20 = TreeQuery<int>(itree, 20);
    REQUIRE(!query20.contains(0));
    REQUIRE(!query20.contains(1));
    REQUIRE(!query20.contains(2));
    REQUIRE(!query20.contains(3));
    REQUIRE(!query20.contains(22));
  }
}

// TEST_CASE("Pattern test 0", "[interval_multitree]") {
//   IntervalMultiTree<IntInterval, int> itree;
//   TreeInsert(itree, IntInterval{0, 100}, 0);
//   for (i32 i = 0; i < 16; i++) {
//     TreeInsert(itree, IntInterval{0, i + 1}, i + 1);
//   }
//   for (i32 i = 0; i < 16; i++) {
//     TreeInsert(itree, IntInterval{i, 16}, i + 1 + 16);
//   }
//   for(i32 i=0; i<16; i++) {
//     REQUIRE(TreeQuery<int>(itree, i).size() == 1 + 16 + 1);
//   }
// }

template<typename I, typename V>
struct BruteforceIntervalTree {
  std::vector<std::pair<I, V>> intervals;

  template<typename T>
  void Insert(I i, T && v) {
    intervals.emplace_back(std::make_pair(i, v));
  }

  template<typename T>
  void Remove(I i, T && v) {
    auto iter = intervals.find(intervals.begin(), intervals.end(), std::make_pair(i, v));
    if(iter != intervals.end()) {
      intervals.erase(iter);
    }
  }
  template<typename T>
  void CollectQueryValues(T p, std::vector<V> &outVec) {
    for(auto& pair : intervals) {
      if(pair.first.Contains(p)) {
        outVec.push_back(pair.second);
      }
    }
  }
};

TEST_CASE("Fuzzy", "[interval_multitree]") {
  BruteforceIntervalTree<IntInterval, int> reference;
  IntervalMultiTree<IntInterval, int> itree;

  TreeInsert(reference, IntInterval{0, 1}, 0);
  TreeInsert(itree, IntInterval{0, 1}, 0);

  REQUIRE(TreeQuery<int>(reference, 0) == TreeQuery<int>(itree, 0));

  constexpr i32 kRangeFrom = 0;
  constexpr i32 kRangeTo = 100;
  constexpr i32 kEntries = 10000;

  std::mt19937 g{0};
  for (i32 i = 0; i < kEntries; i++) {
    auto a = std::uniform_int_distribution<>{kRangeFrom, kRangeTo}(g);
    auto b = std::uniform_int_distribution<>{kRangeFrom, kRangeTo}(g);
    if(a == b) {
      i--;
      continue;
    }
    auto interval = IntInterval{std::min(a, b), std::max(a, b)};
    TreeInsert(reference, interval, i);
    TreeInsert(itree, interval, i);
  }

  for(i32 i=kRangeFrom; i<kEntries; i++) {
    REQUIRE(TreeQuery<int>(reference, i) == TreeQuery<int>(itree, i));
  }
}