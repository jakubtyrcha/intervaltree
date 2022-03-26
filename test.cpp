#include <catch2/catch_test_macros.hpp>
#include "AvlTree.h"
#include <random>
#include <algorithm>
#include <array>

TEST_CASE( "Can read from empty tree and update the root", "[avl_tree]" ) {
    AvlTree tree;
    REQUIRE( tree.Find(0) == std::nullopt );
    tree.Insert(0, 1);
    REQUIRE( tree.Find(0) == 1 );
    REQUIRE( tree.Find(1) == std::nullopt );
}

TEST_CASE( "Can insert 0, 1, 2", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(0, 0);
    tree.Insert(1, 1);
    tree.Insert(2, 2);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
}

TEST_CASE( "Can insert 2, 1, 0", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(2, 2);
    tree.Insert(1, 1);
    tree.Insert(0, 0);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
}

TEST_CASE( "Can insert 1, 0, 2", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(1, 1);
    tree.Insert(0, 0);
    tree.Insert(2, 2);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
}

TEST_CASE( "Can insert monotonically rising sequence", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(0, 0);
    tree.Insert(1, 1);
    tree.Insert(2, 2);
    tree.Insert(3, 3);
    tree.Insert(4, 4);
    tree.Insert(5, 5);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
    REQUIRE( tree.Find(3) == 3 );
    REQUIRE( tree.Find(4) == 4 );
    REQUIRE( tree.Find(5) == 5 );
}

TEST_CASE( "Can insert monotonically falling sequence", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(5, 5);
    tree.Insert(4, 4);
    tree.Insert(3, 3);
    tree.Insert(2, 2);
    tree.Insert(1, 1);
    tree.Insert(0, 0);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
    REQUIRE( tree.Find(3) == 3 );
    REQUIRE( tree.Find(4) == 4 );
    REQUIRE( tree.Find(5) == 5 );
}

TEST_CASE( "Test left left balance case", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(4, 4);
    tree.Insert(2, 2);
    tree.Insert(5, 5);
    tree.Insert(3, 3);
    tree.Insert(1, 1);
    tree.Insert(0, 0);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
    REQUIRE( tree.Find(3) == 3 );
    REQUIRE( tree.Find(4) == 4 );
    REQUIRE( tree.Find(5) == 5 );
    REQUIRE( tree.GetHeight() == 2 );
}

TEST_CASE( "Test right right balance case", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(1, 1);
    tree.Insert(3, 3);
    tree.Insert(0, 0);
    tree.Insert(2, 2);
    tree.Insert(4, 4);
    tree.Insert(5, 5);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
    REQUIRE( tree.Find(3) == 3 );
    REQUIRE( tree.Find(4) == 4 );
    REQUIRE( tree.Find(5) == 5 );
    REQUIRE( tree.GetHeight() == 2 );
}

TEST_CASE( "Test left right balance case", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(4, 4);
    tree.Insert(1, 1);
    tree.Insert(5, 5);
    tree.Insert(0, 0);
    tree.Insert(3, 3);
    tree.Insert(2, 2);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
    REQUIRE( tree.Find(3) == 3 );
    REQUIRE( tree.Find(4) == 4 );
    REQUIRE( tree.Find(5) == 5 );
    REQUIRE( tree.GetHeight() == 2 );
}

TEST_CASE( "Test right left balance case", "[avl_tree]" ) {
    AvlTree tree;
    tree.Insert(1, 1);
    tree.Insert(0, 0);
    tree.Insert(4, 4);
    tree.Insert(3, 3);    
    tree.Insert(5, 5);
    tree.Insert(2, 2);
    REQUIRE( tree.Find(0) == 0 );
    REQUIRE( tree.Find(1) == 1 );
    REQUIRE( tree.Find(2) == 2 );
    REQUIRE( tree.Find(3) == 3 );
    REQUIRE( tree.Find(4) == 4 );
    REQUIRE( tree.Find(5) == 5 );
    REQUIRE( tree.GetHeight() == 2 );
}

TEST_CASE( "Fuzzy tests, 64 elements", "[avl_tree]" ) {
    constexpr i32 kKeysNum = 64;

    std::array<i32, kKeysNum> keys;
    for(i32 i=0; i<kKeysNum; i++) {
        keys[i] = i;
    }
    std::mt19937 g{0};

    const i32 kSequences = 8;
    for(i32 s=0; s<kSequences; s++) {
        std::shuffle(keys.begin(), keys.end(), g);
        AvlTree tree;
        for(i32 i=0; i<kKeysNum; i++) {
            tree.Insert(keys[i], keys[i]);
        }
        for(i32 i=0; i<kKeysNum; i++) {
            REQUIRE( tree.Find(i) == i );
        }
        REQUIRE( tree.GetHeight() <= 7);
    }
}

TEST_CASE( "Fuzzy tests, 1000 elements", "[avl_tree]" ) {
    constexpr i32 kKeysNum = 1000;

    std::array<i32, kKeysNum> keys;
    for(i32 i=0; i<kKeysNum; i++) {
        keys[i] = i;
    }
    std::mt19937 g{0};

    const i32 kSequences = 4;
    for(i32 s=0; s<kSequences; s++) {
        std::shuffle(keys.begin(), keys.end(), g);
        AvlTree tree;
        for(i32 i=0; i<kKeysNum; i++) {
            tree.Insert(keys[i], keys[i]);
        }
        for(i32 i=0; i<kKeysNum; i++) {
            REQUIRE( tree.Find(i) == i );
        }
        REQUIRE( tree.GetHeight() <= 11);
    }
}