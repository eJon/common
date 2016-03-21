// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (gejun) smalltable should not depend on nova/public, so I copy depended
//         files here
// Author: yanlin@baidu.com
// Date: Thu Oct 28 15:20:57 2010

#include <gtest/gtest.h>
#include "st_shared_ptr.hpp"
#include "st_shared_array.hpp"

using namespace st_boost;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_shared_ptr_suite : public ::testing::Test{
protected:
    test_shared_ptr_suite(){};
    virtual ~test_shared_ptr_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

bool g_del_success = false;

struct deleter
{
    void operator () (int *ptr) {
        g_del_success = true;
        delete ptr;
    }
};

class dummy
{
public:
    dummy() {}
    virtual ~dummy() { g_del_success = true; }
};

TEST_F(test_shared_ptr_suite, should_work)
{
    shared_ptr<int> ptr = shared_ptr<int>(new int());
    *ptr = 1;

    //check compiler
    ASSERT_TRUE(ptr);
    ASSERT_EQ(1, *ptr);

    shared_ptr<int> ptr_nul;
    ASSERT_TRUE(!ptr_nul);

    ptr = shared_ptr<int>(new int());
    shared_array<int> ptra = shared_array<int>(new int[123]);
    ptra[1] = 0;
    ptra[0] = 1;
    ASSERT_EQ(0, ptra[1]);
    ASSERT_EQ(1, ptra[0]);
    
    g_del_success = false;
    {
        shared_ptr<int> ptr2 = shared_ptr<int>(new int(), deleter());
        * ptr2 = 1;
    }
    ASSERT_TRUE(g_del_success);

    g_del_success = false;
    {
        shared_ptr<dummy> ptr2 = shared_ptr<dummy>(new dummy());
    }

    ASSERT_TRUE(g_del_success);
}
