// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-09-09 18:24 

#include <fixed_deque.hpp>
#include <gtest/gtest.h>

using namespace st;

class test_usage_suite : public ::testing::Test{
    protected:
        test_usage_suite(){};
        virtual ~test_usage_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};


TEST_F(test_usage_suite, fixed_deque)
{
    typedef FixedDeque<int,10> fq_t;
    fq_t cq;
    for (int j=0; j<100; ++j)
    {
        int r = rand()% 100;
        if (r < 25)
        {
            cq.push_back (rand());
            cout << "push_back, ";
        }
        else if (r < 50)
        {
            cq.pop_front();
            cout << "pop_front, ";
        }
        else if (r < 75)
        {
            cq.push_front (rand());
            cout << "push_front, ";
        }
        else
        {
            cq.pop_back ();
            cout << "pop_back, ";
        }
        cout << "cq=" << show(cq) << endl;
    }
    for (fq_t::iterator it=cq.begin(); it!=cq.end(); ++it)
    {
        cout << "value=" << *it << "\n";
    }
    
}

