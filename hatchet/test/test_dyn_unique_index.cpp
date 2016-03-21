// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Test dyn_unique_index.h
// Author: gejun@baidu.com

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <ext/hash_map>
#include "st_timer.h"
#include "dyn_unique_index.h"

using namespace __gnu_cxx;
using namespace st;
	
int main(int argc, char **argv)
{
    srand (time(0));
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_usage_suite : public ::testing::Test{
protected:
    test_usage_suite(){};
    virtual ~test_usage_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

TEST_F(test_usage_suite, sanity)
{
    DynTupleSchema s;
    s.add_field("apple", FIELD_TYPE_INT, 16);
    s.add_field("pear", FIELD_TYPE_UINT, 16);
    s.add_field("orange", FIELD_TYPE_INT);

    DynUniqueIndex index;
    ASSERT_TRUE(index.not_init());

    std::vector<std::string> key_names;
    key_names.push_back("apple");
    index.init(&s, key_names, 1000, 80);
    ASSERT_FALSE(index.not_init());
    ASSERT_TRUE(index.empty());
    ASSERT_EQ(0ul, index.size());
    
    cout << index << endl;

    SCOPED_DYN_TUPLE(tup1, &s);
    tup1.at("apple") = 1;
    tup1.at("pear") = 10u;
    tup1.at("orange") = 100;
    index.insert(tup1);
    ASSERT_FALSE(index.empty());
    ASSERT_EQ(1ul, index.size());

    SCOPED_DYN_TUPLE(tup2, &s);
    tup2.at("apple") = 2;
    tup2.at("pear") = 20u;
    tup2.at("orange") = 200;
    index.insert(tup2);
    ASSERT_FALSE(index.empty());
    ASSERT_EQ(2ul, index.size());

    SCOPED_DYN_TUPLE(tup3, &s);
    tup3.at("apple") = 3;
    tup3.at("pear") = 30u;
    tup3.at("orange") = 300;
    index.insert(tup3);
    ASSERT_FALSE(index.empty());
    ASSERT_EQ(3ul, index.size());

    tup1.at("apple") = 1;
    tup1.at("pear") = 100u;
    tup1.at("orange") = 10000;
    index.insert(tup1);
    ASSERT_FALSE(index.empty());
    ASSERT_EQ(3ul, index.size());
    
    SCOPED_DYN_TUPLE(key, index.key_schema());
    key.at("apple") = 1;
    DynNormalIterator it1(&s, index.seek(0, key));
    ASSERT_TRUE(it1 && *it1 == tup1);
    ASSERT_FALSE(++it1);

    key.at("apple") = 2;
    DynNormalIterator it2(&s, index.seek(0, key));
    ASSERT_TRUE(it2 && *it2 == tup2);
    ASSERT_FALSE(++it2);

    key.at("apple") = 3;
    DynNormalIterator it3(&s, index.seek(0, key));
    ASSERT_TRUE(it3 && *it3 == tup3);
    ASSERT_FALSE(++it3);

    key.at("apple") = 4;
    ASSERT_FALSE(index.seek(0, key));

    cout << "IndexContent:" << endl;
    for (DynNormalIterator it(&s, index.all()); it != NULL; ++it) {
        cout << *it << endl;
    }
}

