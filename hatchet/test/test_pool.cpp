// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2011-01-05 09:13

#include <gtest/gtest.h>
#include "unused/memory_pool.h"
#include "object_pool.hpp"
#include "st_timer.h"
#include "rc_memory_pool.h"


using namespace st;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


struct Foo {
    int x;
    char buf[];
};

/**
// Brief: 
 **/
class test_usage_suite : public ::testing::Test {
protected:
    test_usage_suite(){};
    virtual ~test_usage_suite(){};
    virtual void SetUp() {
        Foo f;
        cout << &(f.x) << " " << &(f.buf) << " " << sizeof(f) << endl;
    };
    virtual void TearDown() {
    };
};


TEST_F(test_usage_suite, num_of_one)
{
    srand(time(0));
    
    for (int i=0; i<256; ++i) {
        int c = 0;
        for (int j=0; j<8; ++j) {
            if (i & (1<<j)) {
                ++ c;
            }
        }
        ASSERT_EQ (c, num_of_one((uint8_t)i));
    }

    for (int i=0; i<256000; ++i) {
        uint32_t k = rand();
        int c = 0;
        for (int j=0; j<32; ++j) {
            if (k & (1<<j)) {
                ++ c;
            }
        }
        ASSERT_EQ (c, num_of_one(k));
    }
}

struct Dummy {
    char d[48];
};

struct DummyAndID {
    Dummy* p_dummy;
    MP0_ID id;
};

TEST_F(test_usage_suite, perf_memory_pool)
{
    srand(time(0));
    
    MemoryPool mp(sizeof(Dummy));
    typedef ObjectPool<Dummy> mp2_t;
    mp2_t mp2;
    RCMemoryPool rcmp(sizeof(Dummy));
    cout << show(mp2) << endl;
    std::vector<DummyAndID> aa;
    std::vector<MP0_ID> cc;
    std::vector<DummyAndID> bb;
    std::vector<Dummy*> dd;
    std::vector<Dummy*> ee;
    const int LEN = 1000000;
    aa.reserve(LEN);
    bb.reserve(LEN);
    cc.reserve(LEN);
    dd.reserve(LEN);
    ee.reserve(LEN);

    std::vector<u_int> a_rand;
    a_rand.reserve (LEN);
    for (int i=0; i<LEN; ++i) {
        a_rand.push_back (rand());
    }

    cout << "sizeof(Dummy)=" << sizeof(Dummy) << endl;
    
    NaiveTimer t;
    int c;
    //intptr_t s;

    // for (int i=0; i<LEN/2; ++i) {
    //     dd.push_back ((Dummy*)rcmp.alloc());
    //     const MP0_ID id = mp.alloc();
    //     const DummyAndID di = {(Dummy*)mp.address(id), id};
    //     aa.push_back(di);
    // }
    // for (int i=0; i<LEN/2; ++i) {
    //     RCMemoryPool::dereference(dd[i], &rcmp);
    //     mp.dealloc(aa[i].id);
    // }
    // dd.clear();
    // aa.clear();

    t.start ();
    c = 0;
    for (int i=0; i<LEN; ++i, ++c) {
        if (bb.empty() || (a_rand[i] % 100) < 70) {
            const MP0_ID id = mp2.alloc();
            Dummy* p_dummy = (Dummy*) mp2.address(id);
            //++ p_dummy->d[4];
            const DummyAndID di = {p_dummy, id};
            bb.push_back(di);
        }
        else {
            int r = a_rand[i] % bb.size();
            mp2.dealloc (bb[r].id);
            bb[r] = bb.back();
            bb.pop_back();
        }
    }
    t.stop();
    cout << "op.t=" << t.u_elapsed()*1000.0/c << endl;
    cout << show(mp2) << endl;

    
    c = 0;
    t.start ();
    for (int i=0; i<LEN; ++i, ++c) {
        if (dd.empty() || (a_rand[i] % 100) < 70) {
            Dummy* p_dummy = rcmp.alloc_object<Dummy>();
            //++ p_dummy->d[4];
            dd.push_back (p_dummy);
        }
        else {
            int r = a_rand[i] % dd.size();
            RCMemoryPool::dec_ref (dd[r], &rcmp);
            dd[r] = dd.back();
            dd.pop_back();
        }
    }
    t.stop();
    cout << "rcmp.t=" << t.u_elapsed()*1000.0/c << endl;    
    cout << show(rcmp) << endl;

    
    // c = 0;
    // t.start ();
    // for (int i=0; i<LEN; ++i, ++c) {
    //     dd.push_back ((Dummy*)rcmp.alloc());
    // }
    // t.stop();
    // cout << "rcmp.t=" << t.u_elapsed()*1000.0/c << endl;    
    // t.start();
    // c=0;
    // for (int i=0; i<dd.size(); ++i, ++c) {
    //     rcmp.dereference (dd[i]);
    // }
    // t.stop();
    // cout << "rcmp.t=" << t.u_elapsed()*1000.0/c << endl;    
    // cout << show(rcmp) << endl;

    
    c= 0;
    t.start ();
    for (int i=0; i<LEN; ++i, ++c) {
        if (aa.empty() || (a_rand[i] % 100) < 70) {
            const MP0_ID id = mp.alloc();
            const DummyAndID di = {(Dummy*)mp.address(id), id};
            aa.push_back(di);
            //((Dummy*)mp.address(id))->d[0] = 1;
        } else {
            int r = a_rand[i] % aa.size();
            mp.dealloc (aa[r].id);
            aa[r] = aa.back();
            aa.pop_back();
        }
    }
    t.stop();
    cout << "mp.t=" << t.u_elapsed()*1000.0/c << endl;    

    EXPECT_EQ (aa.size(), dd.size());
   

    c= 0;
    t.start ();
    for (int i=0; i<LEN; ++i, ++c) {
        if (ee.empty() || (a_rand[i] % 100) < 70) {
            ee.push_back(new Dummy);
        } else {
            int r = a_rand[i] % ee.size();
            delete ee[r];
            ee[r] = ee.back();
            ee.pop_back();
        }
    }
    t.stop();
    cout << "new.t=" << t.u_elapsed()*1000.0/c << endl;    

        
    t.start ();
    c = 0;
    for (int i=0; i<LEN; ++i, ++c) {
        if (cc.empty() || (a_rand[i] % 100) < 70) {
            cc.push_back (rand());
        } else {
            int r = a_rand[i] % cc.size();
            cc[r] = cc.back();
            cc.pop_back();
        }
    }
    t.stop();
    cout << "vector.t=" << t.u_elapsed()*1000.0/c << endl;

}

