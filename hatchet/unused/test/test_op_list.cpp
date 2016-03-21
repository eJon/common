// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-08-21 10:01

#include <gtest/gtest.h>
#include <set>
#include <st_timer.h>
#include <op_list.hpp>
#include <functional.hpp>

using namespace st;


int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


/**
// Brief: 
 **/
class test_usage_suite : public ::testing::Test{
protected:
    test_usage_suite(){};
    virtual ~test_usage_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};


/**
// Brief: 
 * @begin_version 
 **/
TEST_F(test_usage_suite, perf1)
{
    typedef op_list_t<int> ops_t;
    
    ops_t ops;
    NaiveTimer t;
    const int max_value = 100000;
    const int num = 10000;
    int* rand_seq = new int[num*3];
    for (int i=0; i<num*3; ++i)
    {
        rand_seq[i] = rand();
    }
    int r=-1;
    t.start();
    for (int i=0; i<num; ++i)
    {
        if (rand_seq[++r] % 100 < 50)
        {
            ops.add(rand_seq[++r] % max_value);
        }
        else
        {
            ops.del(rand_seq[++r] % max_value);
        }
    }
    t.stop();
    cout << "t1=" << t.u_elapsed()/(float)num  << "us" << endl;

    size_t sz = ops.size();
    cout << "sz=" << sz << endl;
    
    t.start();
    ops.sort<compare<int> >();
    ops.unique<compare<int> >();
    t.stop();
    cout << "t2=" << t.u_elapsed()/(float)sz  << "us" << endl;
}

