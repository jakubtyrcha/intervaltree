#include "IntervalMultiTree.h"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <random>

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

template <typename I, typename V> struct BruteforceIntervalTree {
  std::vector<std::pair<I, V>> intervals;

  template <typename T> void Insert(I i, T &&v) {
    intervals.emplace_back(std::make_pair(i, v));
  }

  template <typename T> void Remove(I i, T &&v) {
    auto iter =
        std::find(intervals.begin(), intervals.end(), std::make_pair(i, v));
    if (iter != intervals.end()) {
      intervals.erase(iter);
    }
  }
  template <typename T> void CollectQueryValues(T p, std::vector<V> &outVec) {
    for (auto &pair : intervals) {
      if (pair.first.Contains(p)) {
        outVec.push_back(pair.second);
      }
    }
  }
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
std::vector<V> TreeQuery(T &container, E point) {
  std::vector<V> values;
  container.CollectQueryValues(point, values);
  return values;
}

TEST_CASE("Interval multitree short intervals benchmark", "[interval_multitree]") {
  BruteforceIntervalTree<Interval, int> reference;
  IntervalMultiTree<Interval, int> itree;

  // Build trees

  constexpr f32 kRangeFrom = 0.f;
  constexpr f32 kRangeTo = 10000.f;
  constexpr i32 kEntries = 10000;

  int ctr = 0;
  for (f32 x = kRangeFrom; x < kRangeTo; x += 100.f) {
    TreeInsert(reference, Interval{x, x + 100.f}, ctr);
    TreeInsert(itree, Interval{x, x + 100.f}, ctr);
    ctr++;
  }

  std::mt19937 g{0};
  for (i32 i = 0; i < kEntries; i++) {
    f32 len = std::gamma_distribution<f32>{2.f, 1.f}(g);
    len = std::max(len, 0.01f);

    f32 start =
        std::uniform_real_distribution<f32>{kRangeFrom, kRangeTo - 5.f}(g);
    f32 end = std::min(kRangeTo, start + len);
    TreeInsert(reference, Interval{start, end}, ctr);
    TreeInsert(itree, Interval{start, end}, ctr);
    ctr++;
  }

  BENCHMARK_ADVANCED("brute-force list")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([&reference, kRangeFrom, kRangeTo] {
      size_t ctr = 0;
      for (f32 x = kRangeFrom; x < kRangeTo; x += 1.f) {
        ctr += TreeQuery<int>(reference, x).size();
      }
      return ctr;
    });
  };

  BENCHMARK_ADVANCED("interval tree")(Catch::Benchmark::Chronometer meter) {
    meter.measure([&itree, kRangeFrom, kRangeTo] {
      size_t ctr = 0;
      for (f32 x = kRangeFrom; x < kRangeTo; x += 1.f) {
        ctr += TreeQuery<int>(itree, x).size();
      }
      return ctr;
    });
  };
}

TEST_CASE("Interval multitree long intervals benchmark", "[interval_multitree]") {
  BruteforceIntervalTree<Interval, int> reference;
  IntervalMultiTree<Interval, int> itree;

  // Build trees

  constexpr f32 kRangeFrom = 0.f;
  constexpr f32 kRangeTo = 10000.f;
  constexpr i32 kEntries = 10000;

  int ctr = 0;
  for (f32 x = kRangeFrom; x < kRangeTo; x += 100.f) {
    TreeInsert(reference, Interval{x, x + 100.f}, ctr);
    TreeInsert(itree, Interval{x, x + 100.f}, ctr);
    ctr++;
  }

  std::mt19937 g{0};
  for (i32 i = 0; i < kEntries; i++) {
    f32 len = std::gamma_distribution<f32>{2.f, 10.f}(g);
    len = std::max(len, 0.01f);

    f32 start =
        std::uniform_real_distribution<f32>{kRangeFrom, kRangeTo - 5.f}(g);
    f32 end = std::min(kRangeTo, start + len);
    TreeInsert(reference, Interval{start, end}, ctr);
    TreeInsert(itree, Interval{start, end}, ctr);
    ctr++;
  }

  BENCHMARK_ADVANCED("brute-force list")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([&reference, kRangeFrom, kRangeTo] {
      size_t ctr = 0;
      for (f32 x = kRangeFrom; x < kRangeTo; x += 1.f) {
        ctr += TreeQuery<int>(reference, x).size();
      }
      return ctr;
    });
  };

  BENCHMARK_ADVANCED("interval tree")(Catch::Benchmark::Chronometer meter) {
    meter.measure([&itree, kRangeFrom, kRangeTo] {
      size_t ctr = 0;
      for (f32 x = kRangeFrom; x < kRangeTo; x += 1.f) {
        ctr += TreeQuery<int>(itree, x).size();
      }
      return ctr;
    });
  };
}