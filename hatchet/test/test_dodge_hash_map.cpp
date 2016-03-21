// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-07-28 08:38 

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <ext/hash_map>
#include "dodge_hash_map.hpp"
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
        srand (time(0));
    };
    virtual void TearDown() {
    };
};
 
TEST_F(test_usage_suite, DodgeHashMap)
{
    srand (0);

    // union {
    //     struct {
    //         uint32_t next : 30;  // 2^30 = 1G nodes, enough for daily use
    //         uint32_t status : 2;  // UNUSED/HASHED/CHAINED
    //     };
    //     uint32_t dummy;
    // } test;
    // test.status = 1;
    // test.next = 29;
    // cout << test.dummy << endl;
    // return;
    
    hash_map<int,int> ref(10000);
    DodgeHashMap<int,int> ht;
    ht.init ();
    // cout << "ht: " << show(ht) << endl;

    // ht.insert(1,1);
    // cout << "ht: " << show(ht) << endl;

    // ht.insert(2,2);
    // cout << "ht: " << show(ht) << endl;

    // ht.erase(2);
    // cout << "ht: " << show(ht) << endl;
    
    for (int j=0; j<30; ++j) {
        for (int i=0; i<50000; ++i) {
            int k = rand() % 0xFFFF;
            int p = rand () % 1000;
            if (p < 600) {
                ht.insert (k,i);
                ref[k] = i;
            } else if (p < 999) {
                ht.erase (k);
                ref.erase (k);
            } else {
                ht.clear();
                ref.clear();
            }
        }

        // bi-check
        printf ("ht.size=%lu, ref.size=%lu\n", ht.size(), ref.size());
        for (DodgeHashMap<int,int>::iterator it=ht.begin(); it!=ht.end(); ++it)
        {
            hash_map<int,int>::iterator it2 = ref.find(it->key);
            ASSERT_TRUE (it2 != ref.end());
            ASSERT_EQ (it2->second, it->value);
        }
        for (hash_map<int,int>::iterator it=ref.begin(); it!=ref.end(); ++it)
        {
            int* p_value = ht.seek(it->first);
            ASSERT_TRUE (p_value != NULL);
            ASSERT_EQ (it->second, *p_value);
        }
        
        ASSERT_EQ (ht.size(), ref.size());

        //cout << "modify done" << endl;
    }

    cout << "ht=" << show(ht) << endl;

}


TEST_F(test_usage_suite, perf_hashmap)
{
    const size_t initial_bucket_num  = 10000000;
    const size_t n_value = 3000000;
    
    DodgeHashMap<int, u_long> cellar_ht;
    cellar_ht.init(initial_bucket_num);
    
    std::vector<std::pair<int, u_long> > values;
    std::vector<int> seq;

    {
        TIME_SCOPE_IN_MS("Prepare_random_values: ");
        for (size_t i = 0; i < n_value; ++i) {
            int k = rand();
            int v = rand();
            values.push_back (std::pair<int,u_long>(k, v));
        }
    }
        
    {
        TIME_SCOPE_IN_NS("Insert_cellar_map: ", values.size());
        for (size_t i = 0; i < values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];    
            cellar_ht.insert (p.first, p.second);
        }
    }

    {
        TIME_SCOPE_IN_MS("Prepare_random_sequence: ");    
        seq.reserve(values.size());
        for (size_t i = 0; i < values.size(); ++i) {
            seq.push_back (i);
        }
        std::random_shuffle (seq.begin(), seq.end());
    }
    
    {
        TIME_SCOPE_IN_MS("erase_hash_maps: ");    
        for (long i=static_cast<long>(values.size())/2; i>=0; --i) {
            int r = values[seq[i]].first;
            cellar_ht.erase (r);
        }
    }
        
    cout << " cellar:"  << show(cellar_ht) << endl;
    
    std::random_shuffle (values.begin(), values.end());

    int v;
    u_long *p;    

    {
        TIME_SCOPE_IN_NS_NO_NL("Seek_cellar_map: ", values.size());
        v = 0;
        for (size_t i = 0; i < values.size(); ++i) {
            p = cellar_ht.seek (values[i].first);
            if (p) {
                v += *p;
            }
        }
    }
    cout << ", v=" << v << endl;

    {
        TIME_SCOPE_IN_NS_NO_NL("Seek_vector: ", values.size());  
        v = 0;
        for (size_t i = 0; i < seq.size(); ++i) {
            v += values[seq[i]].first;
        }
    }
    cout << ", v=" << v << endl;
    
}


