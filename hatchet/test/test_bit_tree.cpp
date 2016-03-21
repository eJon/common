// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-12-15

#include <gtest/gtest.h>
#include <bit_tree.h>
#include <vector>
#include <st_timer.h>
#include <deque>

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

typedef vector<bool> Ref;

static int vector_ctz (const Ref& vec)
{
    for (size_t i=0; i<vec.size(); ++i) {
        if (!vec[i]) {
            return i;
        }
    }
    return vec.size();
}

TEST_F(test_usage_suite, random)
{
    srand(time(0));
    //srand(0);

    const int LEN = 504302;
    BitTree bt1(LEN);
    Ref ref;
    ref.reserve (LEN);
    for (size_t i=0; i<ref.capacity(); ++i) {
        ref.push_back (false);
    }
    
    ASSERT_TRUE (BitTree::N_ELEM_BIT == 64);
    ASSERT_EQ (bt1.orig_len_, (u_int)LEN);
    cout << "level=" << bt1.level_
         << " n_elem=" << bt1.n_elem_
         << " n_meta_elem=" << bt1.n_meta_elem_
         << endl;
    // EXPECT_EQ (bt1.n_elem_, 7);
    // EXPECT_EQ (bt1.n_meta_elem_, 1);

    //cout << show (bt1) << endl;
    
    for (int i=0; i<63;++i) {
        bt1.set(i, 1);
        ref[i] = true;
    }
    ASSERT_EQ (bt1.find_first_zero(), vector_ctz(ref));
    
    bt1.set(63,1);
    ref[63] = true;
    ASSERT_EQ (vector_ctz(ref), bt1.find_first_zero());
    
    // bt1.set(36,0);
    // cout << show (bt1) << endl;
    // cout << "fz=" << bt1.find_first_zero() << endl;

    for (int i=64; i<127;++i) {
        bt1.set(i, 1);
        ref[i] = 1;
    }
    ASSERT_EQ (vector_ctz(ref), bt1.find_first_zero());

    bt1.set(127,1);
    ref[127] = true;
    ASSERT_EQ (vector_ctz(ref), bt1.find_first_zero());

    bt1.set(88,0);
    ref[88] = false;
    ASSERT_EQ (vector_ctz(ref), bt1.find_first_zero());

    for (int i=0; i<10000; ++i) {
        int r = rand() % LEN;
        bool b = rand() % 2;
        bt1.set(r, b);
        ref[r] = b;
        ASSERT_EQ (vector_ctz(ref), bt1.find_first_zero());
    }

    NaiveTimer t;
    const int REP = 100000;
    int s = 0;
    t.start ();
    for (int i=0; i<REP; ++i) {
        int r = rand() % LEN;
        bool b = rand() % 2;
        s += r + b;
    }
    t.stop ();
    unsigned long e1 = t.u_elapsed();
    cout << "t=" << e1*1000.0/REP << "ns " << s << endl;

    s = 0;
    t.start ();
    for (int i=0; i<REP; ++i) {
        int r = rand() % LEN;
        bool b = rand() % 2;
        bt1.set(r, b);
        s += bt1.find_first_zero();
    }
    t.stop();

    cout << "t=" << (t.u_elapsed()-e1)*1000.0/REP << "ns " << s << endl;

    std::deque<u_int> q;
    for (int i=0; i<LEN; ++i) {
        q.push_back (rand());
    }

    s = 0;
    t.start ();
    while (!q.empty()) {
        s += q.front();
        q.pop_front ();
    }
    t.stop();
    cout << "t=" << t.u_elapsed()*1000.0/LEN << "ns " << s << endl;

}

