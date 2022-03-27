#include "AvlTree.h"
#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <random>

TEST_CASE("Can read from empty tree and update the root", "[avl_tree]") {
  AvlTree tree;
  REQUIRE(tree.Find(0) == std::nullopt);
  tree.Insert(0, 1);
  REQUIRE(tree.Find(0) == 1);
  REQUIRE(tree.Find(1) == std::nullopt);
}

TEST_CASE("Can get tree height", "[avl_tree]") {
  AvlTree tree;
  REQUIRE(tree.GetHeight() == 0);
  tree.Insert(1, 1);
  REQUIRE(tree.GetHeight() == 1);
  tree.Insert(0, 0);
  tree.Insert(2, 2);
  REQUIRE(tree.GetHeight() == 2);
  tree.Insert(3, 3);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Can insert 0, 1, 2", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(0, 0);
  tree.Insert(1, 1);
  tree.Insert(2, 2);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
}

TEST_CASE("Can insert 2, 1, 0", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(2, 2);
  tree.Insert(1, 1);
  tree.Insert(0, 0);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
}

TEST_CASE("Can insert 1, 0, 2", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(1, 1);
  tree.Insert(0, 0);
  tree.Insert(2, 2);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
}

TEST_CASE("Can insert monotonically rising sequence", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(0, 0);
  tree.Insert(1, 1);
  tree.Insert(2, 2);
  tree.Insert(3, 3);
  tree.Insert(4, 4);
  tree.Insert(5, 5);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
}

TEST_CASE("Can insert monotonically falling sequence", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(5, 5);
  tree.Insert(4, 4);
  tree.Insert(3, 3);
  tree.Insert(2, 2);
  tree.Insert(1, 1);
  tree.Insert(0, 0);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
}

TEST_CASE("Test left left balance case", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(4, 4);
  tree.Insert(2, 2);
  tree.Insert(5, 5);
  tree.Insert(3, 3);
  tree.Insert(1, 1);
  tree.Insert(0, 0);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Test right right balance case", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(1, 1);
  tree.Insert(3, 3);
  tree.Insert(0, 0);
  tree.Insert(2, 2);
  tree.Insert(4, 4);
  tree.Insert(5, 5);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Test left right balance case", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(4, 4);
  tree.Insert(1, 1);
  tree.Insert(5, 5);
  tree.Insert(0, 0);
  tree.Insert(3, 3);
  tree.Insert(2, 2);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Test right left balance case", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(1, 1);
  tree.Insert(0, 0);
  tree.Insert(4, 4);
  tree.Insert(3, 3);
  tree.Insert(5, 5);
  tree.Insert(2, 2);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Fuzzy insertion tests, 64 elements", "[avl_tree]") {
  constexpr i32 kKeysNum = 64;

  std::array<i32, kKeysNum> keys;
  for (i32 i = 0; i < kKeysNum; i++) {
    keys[i] = i;
  }
  std::mt19937 g{0};

  const i32 kSequences = 8;
  for (i32 s = 0; s < kSequences; s++) {
    std::shuffle(keys.begin(), keys.end(), g);
    AvlTree tree;
    for (i32 i = 0; i < kKeysNum; i++) {
      tree.Insert(keys[i], keys[i]);
    }
    for (i32 i = 0; i < kKeysNum; i++) {
      REQUIRE(tree.Find(i) == i);
    }
    REQUIRE(tree.GetHeight() <= 7);
  }
}

TEST_CASE("Fuzzy insertion tests, 1000 elements", "[avl_tree]") {
  constexpr i32 kKeysNum = 1000;

  std::array<i32, kKeysNum> keys;
  for (i32 i = 0; i < kKeysNum; i++) {
    keys[i] = i;
  }
  std::mt19937 g{0};

  const i32 kSequences = 4;
  for (i32 s = 0; s < kSequences; s++) {
    std::shuffle(keys.begin(), keys.end(), g);
    AvlTree tree;
    for (i32 i = 0; i < kKeysNum; i++) {
      tree.Insert(keys[i], keys[i]);
    }
    for (i32 i = 0; i < kKeysNum; i++) {
      REQUIRE(tree.Find(i) == i);
    }
    REQUIRE(tree.GetHeight() <= 12);
  }
}

TEST_CASE("Can remove the last node", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(0, 0);
  tree.Remove(0);
  REQUIRE(tree.Find(0) == std::nullopt);
}

TEST_CASE("Can remove from a subtree with left child", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(2, 2);
  tree.Insert(1, 1);
  tree.Insert(3, 3);
  tree.Insert(0, 0);
  tree.Remove(1);
  REQUIRE(tree.Find(1) == std::nullopt);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.GetNodesCount() == 3);
}

TEST_CASE("Can remove from a subtree with right child", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(2, 2);
  tree.Insert(0, 0);
  tree.Insert(3, 3);
  tree.Insert(1, 1);
  tree.Remove(0);
  REQUIRE(tree.Find(0) == std::nullopt);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.GetNodesCount() == 3);
}

TEST_CASE("Can remove from a subtree with left and right children",
          "[avl_tree]") {
  AvlTree tree;
  tree.Insert(1, 1);
  tree.Insert(0, 0);
  tree.Insert(3, 3);
  tree.Insert(2, 2);
  tree.Insert(4, 4);
  REQUIRE(tree.GetNodesCount() == 5);
  tree.Remove(3);
  REQUIRE(tree.Find(3) == std::nullopt);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.GetNodesCount() == 4);
}

TEST_CASE("Test removal left left balance case", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(4, 4);
  tree.Insert(2, 2);
  tree.Insert(6, 6);
  tree.Insert(1, 1);
  tree.Insert(3, 3);
  tree.Insert(5, 5);
  tree.Insert(0, 0);
  REQUIRE(tree.GetHeight() == 4);
  tree.Remove(5);
  REQUIRE(tree.Find(5) == std::nullopt);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(6) == 6);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Test removal right right balance case", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(2, 2);
  tree.Insert(1, 1);
  tree.Insert(4, 4);
  tree.Insert(0, 0);
  tree.Insert(3, 3);
  tree.Insert(5, 5);
  tree.Insert(6, 6);
  REQUIRE(tree.GetHeight() == 4);
  tree.Remove(6);
  REQUIRE(tree.Find(6) == std::nullopt);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Test removal left right balance case", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(4, 4);
  tree.Insert(1, 1);
  tree.Insert(5, 5);
  tree.Insert(0, 0);
  tree.Insert(3, 3);
  tree.Insert(6, 6);
  tree.Insert(2, 2);
  REQUIRE(tree.GetHeight() == 4);
  tree.Remove(6);
  REQUIRE(tree.Find(6) == std::nullopt);
  REQUIRE(tree.Find(0) == 0);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Test removal right left balance case", "[avl_tree]") {
  AvlTree tree;
  tree.Insert(2, 2);
  tree.Insert(1, 1);
  tree.Insert(5, 5);
  tree.Insert(0, 0);
  tree.Insert(4, 4);
  tree.Insert(6, 6);
  tree.Insert(3, 3);
  REQUIRE(tree.GetHeight() == 4);
  tree.Remove(0);
  REQUIRE(tree.Find(0) == std::nullopt);
  REQUIRE(tree.Find(1) == 1);
  REQUIRE(tree.Find(2) == 2);
  REQUIRE(tree.Find(3) == 3);
  REQUIRE(tree.Find(4) == 4);
  REQUIRE(tree.Find(5) == 5);
  REQUIRE(tree.Find(6) == 6);
  REQUIRE(tree.GetHeight() == 3);
}

TEST_CASE("Can remove a node with a subtree of 2 children and 4 grandchildren",
          "[avl_tree]") {
  AvlTree tree;
  tree.Insert(3, 3);
  tree.Insert(2, 2);
  tree.Insert(7, 7);
  tree.Insert(0, 0);
  tree.Insert(1, 1);
  tree.Insert(6, 6);
  tree.Insert(9, 9);
  tree.Insert(4, 4);
  tree.Insert(5, 5);
  tree.Insert(8, 8);
  tree.Insert(10, 10);
  tree.Remove(7);
  REQUIRE(tree.Find(7) == std::nullopt);
  for (i32 i = 0; i < 10; i++) {
    if (i != 7) {
      REQUIRE(tree.Find(i) == i);
    }
  }
  REQUIRE(tree.GetNodesCount() == 10);
}

TEST_CASE("Test insertion and removal of monotonically rising sequences",
          "[avl_tree]") {
  AvlTree tree;
  constexpr i32 kKeysNum = 64;
  for (i32 i = 0; i < kKeysNum; i++) {
    tree.Insert(i, i);
  }
  for (i32 i = 0; i < kKeysNum; i++) {
    REQUIRE(tree.Find(i) == i);
    tree.Remove(i);
    REQUIRE(tree.Find(i) == std::nullopt);
    REQUIRE(tree.GetNodesCount() == kKeysNum - i - 1);
  }
}

TEST_CASE("Test insertion of a monotonically rising sequence and removal of"
          "a monotonically falling sequence",
          "[avl_tree]") {
  AvlTree tree;
  constexpr i32 kKeysNum = 64;
  for (i32 i = 0; i < kKeysNum; i++) {
    tree.Insert(i, i);
  }
  for (i32 i = kKeysNum - 1; i >= 0; i--) {
    REQUIRE(tree.Find(i) == i);
    tree.Remove(i);
    REQUIRE(tree.Find(i) == std::nullopt);
    REQUIRE(tree.GetNodesCount() == i);
  }
}

TEST_CASE("Test insertion and removal of monotonically falling sequences",
          "[avl_tree]") {
  AvlTree tree;
  constexpr i32 kKeysNum = 64;
  for (i32 i = kKeysNum - 1; i >= 0; i--) {
    tree.Insert(i, i);
  }
  for (i32 i = kKeysNum - 1; i >= 0; i--) {
    REQUIRE(tree.Find(i) == i);
    tree.Remove(i);
    REQUIRE(tree.Find(i) == std::nullopt);
    REQUIRE(tree.GetNodesCount() == i);
  }
}

TEST_CASE("Test insertion of a monotonically falling sequence and removal of "
          "a monotonically rising sequence",
          "[avl_tree]") {
  AvlTree tree;
  constexpr i32 kKeysNum = 64;
  for (i32 i = kKeysNum - 1; i >= 0; i--) {
    tree.Insert(i, i);
  }
  for (i32 i = 0; i < kKeysNum; i++) {
    REQUIRE(tree.Find(i) == i);
    tree.Remove(i);
    REQUIRE(tree.Find(i) == std::nullopt);
    REQUIRE(tree.GetNodesCount() == kKeysNum - i - 1);
  }
}

TEST_CASE("Fuzzy removal test, 64 elements", "[avl_tree]") {
  constexpr i32 kKeysNum = 64;

  std::array<i32, kKeysNum> keys;
  for (i32 i = 0; i < kKeysNum; i++) {
    keys[i] = i;
  }
  std::mt19937 g{0};

  const i32 kSequences = 8;
  for (i32 s = 0; s < kSequences; s++) {
    std::shuffle(keys.begin(), keys.end(), g);
    AvlTree tree;
    for (i32 i = 0; i < kKeysNum; i++) {
      tree.Insert(keys[i], keys[i]);
    }
    std::shuffle(keys.begin(), keys.end(), g);
    for (i32 i = 0; i < kKeysNum; i++) {
      REQUIRE(tree.Find(keys[i]) == keys[i]);
      tree.Remove(keys[i]);
      REQUIRE(tree.Find(keys[i]) == std::nullopt);
      REQUIRE(tree.GetNodesCount() == kKeysNum - i - 1);
    }
  }
}

TEST_CASE("Fuzzy removal test, 1000 elements", "[avl_tree]") {
  constexpr i32 kKeysNum = 1000;

  std::array<i32, kKeysNum> keys;
  for (i32 i = 0; i < kKeysNum; i++) {
    keys[i] = i;
  }
  std::mt19937 g{0};

  const i32 kSequences = 4;
  for (i32 s = 0; s < kSequences; s++) {
    std::shuffle(keys.begin(), keys.end(), g);
    AvlTree tree;
    for (i32 i = 0; i < kKeysNum; i++) {
      tree.Insert(keys[i], keys[i]);
    }
    std::shuffle(keys.begin(), keys.end(), g);
    for (i32 i = 0; i < kKeysNum; i++) {
      REQUIRE(tree.Find(keys[i]) == keys[i]);
      tree.Remove(keys[i]);
      REQUIRE(tree.Find(keys[i]) == std::nullopt);
      REQUIRE(tree.GetNodesCount() == kKeysNum - i - 1);
    }
  }
}