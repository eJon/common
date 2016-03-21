// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Test cow_hash_map.hpp
// Author: gejun@baidu.com

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <ext/hash_map>
#include "cow_hash_map.hpp"
#include "st_timer.h"

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

struct HugeValue {
    char data[84];
};

TEST_F(test_usage_suite, resize_mem)
{
    typedef CowHashMap<u_int, HugeValue> Map;
    for (int j = 0; j < 2; ++j) {
        Map m;
        if (j == 1) {
            m.init(12582917);
            cout << "Initially resize to 12582917" << endl;
        } else {
            m.init();
        }
        
        const long N = 5000000;

        for (long i = 0; i < N; ++i) {
            m.insert(rand(), HugeValue());
        }
        cout << "After inserted " << N << " values, mem = " << m.mem() << endl;
        //cout << show(m) << endl;

        Map m2 = m;
        cout << "After a copy, mem = " << (m.mem() + m2.mem()) << endl;

        const long M = (long)(N * 0.01);
        for (long i = 0; i < M; ++i) {
            m.insert(rand(), HugeValue());
        }
        cout << "Inserted another " << M << " values, mem = "
             << (m.mem() + m2.mem()) << endl;
    }
}

int n_con = 0;
int n_cp_con = 0;
int n_des = 0;
int n_cp = 0;

struct Value {
    Value () : x_(0) {
        ++ n_con;
    }

    Value (int x) : x_(x) {
        ++ n_con;
    }
    
    Value (const Value& rhs) : x_(rhs.x_) {
        ++ n_cp_con;
    }
    
    ~Value() { ++ n_des; }
    
    Value& operator= (const Value& rhs) {
        x_ = rhs.x_;
        ++ n_cp;
        return *this;
    }

    bool operator== (const Value& rhs) const { return x_ == rhs.x_; }
    bool operator!= (const Value& rhs) const { return x_ != rhs.x_; }

friend ostream& operator<< (ostream& os, const Value& v)
    { return os << v.x_; }

    int x_;
};

TEST_F(test_usage_suite, sanity)
{
    typedef CowHashMap<int, long> Map;
    Map m;

    ASSERT_TRUE(m.not_init());
    m.init(1000, 70);
    ASSERT_FALSE(m.not_init());
    ASSERT_EQ(m.size(), 0ul);
    ASSERT_TRUE(m.empty());

    // Initial insertion
    m.insert(1, 10);
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    Map::Pointer p = m.seek(1);
    ASSERT_TRUE(p && *p == 10);

    // Override
    m.insert(1, 100);
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    p = m.seek(1);
    ASSERT_TRUE(p && *p == 100);
    
    // Insert anthter
    m.insert(2, 20);
    ASSERT_EQ(m.size(), 2ul);
    ASSERT_FALSE(m.empty());
    p = m.seek(2);
    ASSERT_TRUE(p && *p == 20);

    // Erase exist
    m.erase(1);
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    ASSERT_FALSE(m.seek(1));

    // Clear
    m.clear();
    ASSERT_EQ(m.size(), 0ul);
    ASSERT_TRUE(m.empty());
    ASSERT_FALSE(m.seek(1));
    ASSERT_FALSE(m.seek(2));
}


// Randomly insert/erase/clear a CowHashMap while making snapshots
TEST_F(test_usage_suite, random_insert_erase)
{
    srand (0);

    hash_map<int, Value> ref[2];
    typedef CowHashMap<int, Value> Map;
    Map ht[2];
    ht[0].init (40, 80);
    cout << show(ht[0]) << endl;
        
    ht[1] = ht[0];
    cout << show(ht[1]) << endl;

    for (int j = 0; j < 30; ++j) {
        // Make snapshot
        ht[1] = ht[0];
        ref[1] = ref[0];

        for (int i = 0; i < 100000; ++i) {
            int k = rand() % 0xFFFF;
            int p = rand() % 1000;
            if (p < 600) {
                ht[0].insert(k, i);
                ref[0][k] = i;
            } else if(p < 999) {
                ht[0].erase (k);
                ref[0].erase (k);
            } else {
                ht[0].clear();
                ref[0].clear();
            }
        }
        
        // bi-check
        for (int i=0; i<2; ++i) {
            for (CowHashMap<int, Value>::Iterator it = ht[i].begin();
                 it != ht[i].end(); ++it)
            {
                hash_map<int, Value>::iterator it2 = ref[i].find(it->first);
                ASSERT_TRUE (it2 != ref[i].end());
                ASSERT_EQ (it2->second, it->second);
            }
        
            for (hash_map<int, Value>::iterator it = ref[i].begin();
                 it != ref[i].end(); ++it)
            {
                Value* p_value = ht[i].seek(it->first);
                ASSERT_TRUE (p_value != NULL);
                ASSERT_EQ (it->second, p_value->x_);
            }
        
            ASSERT_EQ (ht[i].size(), ref[i].size());
        }
    }

    cout << "ht[0] = " << show(ht[0]) << endl
         << "ht[1] = " << show(ht[1]) << endl;

    ht[0].clear ();
    ht[1].clear ();
    ref[0].clear ();
    ref[1].clear ();

    ASSERT_EQ (ht[0].alloc()->alloc_num(), 0ul);
    ASSERT_EQ (n_con + n_cp_con, n_des);

    cout << "n_con:" << n_con << endl
         << "n_cp_con:" << n_cp_con << endl
         << "n_con+n_cp_con:" <<  n_con+n_cp_con <<  endl
         << "n_des:" << n_des << endl
         << "n_cp:" << n_cp << endl
        ;    
}

// This test case inserts items into bucket[0] so that all itmes
// are chained together. As a result, we can test copy_on_write 
// behavior of the list under this extreme situation.
// Author: jiangrujie@baidu.com
TEST_F(test_usage_suite, copy_on_write)
{
    srand(time(0));
    const int BUCKET = 53;
    const int THRESHOLD = 40;

    typedef CowHashMap<int,int> Map;
    
    Map a_ht[4];

    a_ht[0].init(BUCKET, 100);
    for(int i = 0; i < THRESHOLD; i++) {
        a_ht[0].insert(i * BUCKET, i);
    }

    // All maps share the same node by using operator=.
    for(int i = 1; i < 4; i++) {
        a_ht[i] = a_ht[0];
    }

    for(int i = 0; i < 3; i++) {
        int update_key = rand() % THRESHOLD * BUCKET;
        int new_key = THRESHOLD + rand() % 100 * BUCKET;
        int erase_key = rand() % THRESHOLD * BUCKET;

        // a_ht[0] update the item that already exists in the list.
        a_ht[0].insert(update_key, -1);
        ASSERT_EQ(*(a_ht[0].seek(update_key)), -1);
        ASSERT_EQ(*(a_ht[1].seek(update_key)), update_key / BUCKET);
        ASSERT_EQ(*(a_ht[2].seek(update_key)), update_key / BUCKET);
        // Revert.
        a_ht[0].insert(update_key, update_key / BUCKET);

        // a_ht[1] insert new item into the map.
        a_ht[1].insert(new_key, new_key);
        ASSERT_TRUE(a_ht[0].seek(new_key) == NULL);
        ASSERT_EQ(*(a_ht[1].seek(new_key)), new_key);
        ASSERT_TRUE(a_ht[2].seek(new_key) == NULL);

        // a_ht[2] erase the item that already exists.
        a_ht[2].erase(erase_key);
        ASSERT_EQ(*(a_ht[0].seek(erase_key)), erase_key / BUCKET);
        ASSERT_EQ(*(a_ht[1].seek(erase_key)), erase_key / BUCKET);
        ASSERT_TRUE(a_ht[2].seek(erase_key) == NULL);
        // Revert.
        a_ht[2].insert(erase_key, erase_key / BUCKET);
        
        // a_ht[3] resize itself.
        ASSERT_TRUE(a_ht[3].resize(100 + i * 1000)) << "Out of memory.";
        for(int j = 0; j < THRESHOLD; j++) {
            ASSERT_EQ(*(a_ht[3].seek(j * BUCKET)), j);
        }
    }

    for(int i = 0 ; i < 4; i++) {
        cout << "a_ht[" << i << "] = " << show(a_ht[i]) << endl;
    }
    
    for(int i = 0 ; i < 4; i++) {
        a_ht[i].clear();
    }
    ASSERT_EQ (a_ht[0].alloc()->alloc_num(), 0ul);
}


