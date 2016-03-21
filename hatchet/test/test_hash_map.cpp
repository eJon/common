// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-07-28 08:38 

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <ext/hash_map>
#include "linear_hash_map.hpp"
#include "coalesced_hash_map.hpp"
#include "cow_hash_map.hpp"
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
    };
    virtual void TearDown() {
    };
};

// Brief: simple usage
// Author: gejun@baidu.com
TEST_F(test_usage_suite, perf_hashmap)
{
    srand (time(0));

    const size_t initial_bucket_num  = 5000000;
    const size_t n_value = 5000000;
    
    CowHashMap<int, u_long> cow2_ht;
    cow2_ht.init(initial_bucket_num);

    CoalescedHashMap<int, u_long> cellar_ht;
    cellar_ht.create(initial_bucket_num);

    DodgeHashMap<int, u_long> dodge_ht;
    dodge_ht.init(initial_bucket_num);
    
    LinearHashMap<int, u_long> internal_ht;
    internal_ht.create (1000000, 60, true);
    
    std::vector<std::pair<int, u_long> > values;

    {
        TIME_SCOPE_IN_MS("Prepare_random_values: ");
                    
        for (size_t i=0; i<n_value; ++i) {
            int k = rand();
            int v = rand();
            values.push_back (std::pair<int,u_long>(k, v));
        }
    }
        
    {
        TIME_SCOPE_IN_NS("Insert_cow_hash_map2: ", values.size());
        for (size_t i=0; i<values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];
            cow2_ht.insert (p.first, p.second);
        }
    }
    
    {
        TIME_SCOPE_IN_NS("Insert_cellar_map: ", values.size());
        for (size_t i=0; i<values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];    
            cellar_ht.insert (p.first, p.second);
        }
    }

    {
        TIME_SCOPE_IN_NS("Insert_dodge_map: ", values.size());
        for (size_t i=0; i<values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];    
            dodge_ht.insert (p.first, p.second);
        }
    }
    
    {
        TIME_SCOPE_IN_NS("Insert_open_addressing_hash_map: ", values.size());    
        for (size_t i=0; i<values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];
            internal_ht.insert (p.first, p.second);
        }
    }

    std::vector<int> seq;
    {
        TIME_SCOPE_IN_MS("Prepare_random_sequence: ");    
        seq.reserve (values.size());
        for (size_t i=0; i<values.size(); ++i) {
            seq.push_back (i);
        }
        std::random_shuffle (seq.begin(), seq.end());
    }
    
    {
        TIME_SCOPE_IN_MS("randomly insert/erase hashmaps: ");    
        for (long i=static_cast<long>(values.size())/2; i>=0; --i) {
            if (seq[i] % 3 == 0) {
                int r = values[seq[i]].first;
                cow2_ht.erase(r);
                cellar_ht.erase(r);
                dodge_ht.erase(r);
                internal_ht.erase(r);
            } else {
                int k = rand();
                int v = rand();
                cow2_ht.insert(k, v);
                cellar_ht.insert(k, v);
                dodge_ht.insert(k, v);
                internal_ht.insert(k, v);
            }
        }
    }
        
    cout << " cow2:" << show(cow2_ht) << endl
         << " cellar:"  << show(cellar_ht) << endl
         << " dodge:"  << show(dodge_ht) << endl
         << " internal:{mem=" << internal_ht.mem()
         << " content=" << show(internal_ht) << "}" << endl
        ;
    
    std::random_shuffle (values.begin(), values.end());

    int v;
    u_long *p;    

    {
        TIME_SCOPE_IN_NS_NO_NL("Seek_dodge_map: ", values.size());
        v = 0;
        for (size_t i=0; i<values.size(); ++i) {
            p = dodge_ht.seek(values[i].first);
            if (p) {
                v += *p;
            }
        }
    }
    cout << ", v=" << v << endl;

    {
        TIME_SCOPE_IN_NS_NO_NL("Seek_cellar_map: ", values.size());
        v = 0;
        for (size_t i=0; i<values.size(); ++i) {
            p = cellar_ht.seek (values[i].first);
            if (p) {
                v += *p;
            }
        }
    }
    cout << ", v=" << v << endl;
    
    {
        TIME_SCOPE_IN_NS_NO_NL("Seek_cow_hash_map2: ", values.size());
        v = 0;
        for (size_t i=0; i<values.size(); ++i) {
            p = cow2_ht.seek (values[i].first);
            if (p) {
                v += *p;
            }
        }
    }
    cout << ", v=" << v << endl;

    {
        TIME_SCOPE_IN_NS_NO_NL("Seek_open_addressing_hash_map: ", values.size());
        v = 0;
        for (size_t i=0; i<values.size(); ++i) {
            p = internal_ht.seek (values[i].first);
            if (p) {
                v += *p;
            }
        }
    }
    cout << ", v=" << v << endl;

    {
        TIME_SCOPE_IN_NS_NO_NL("Seek_vector: ", values.size());  
        v = 0;
        for (size_t i=0; i<seq.size(); ++i) {
            v += values[seq[i]].first;
        }
    }
    cout << ", v=" << v << endl;
    
}


// Brief: check CoalescedHashMap
// Author: gejun@baidu.com
TEST_F(test_usage_suite, CoalescedHashMap)
{
    srand (time(0));
    
    hash_map<int,int> ref(10000);

    CoalescedHashMap<int,int> ht;
    ht.create ();
    cout << show(ht) << endl;
    
    for (int j=0; j<30; ++j) {
        for (int i=0; i<100000; ++i) {
            int k = rand() % 0xFFFF;
            int p = rand () % 1000;
            if (p < 600) {
                ht.insert (k,i);
                ref[k] = i;
            }
            else if (p < 999) {
                ht.erase(k);
                ref.erase(k);
            }
            else {
                ht.clear();
                ref.clear();
            }
        }
                
        // bi-check
        printf ("ht.size=%lu, ref.size=%lu\n", ht.size(), ref.size());
        for (CoalescedHashMap<int,int>::iterator it=ht.begin(); it!=ht.end(); ++it)
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

// Brief: simple usage of ohashmap
// Author: gejun@baidu.com
TEST_F(test_usage_suite, simply_use_ohashmap)
{
    srand (time(0));
    
    LinearHashMap<int, int> ht;
    ht.create(100,50);
    for (int i=0; i<10; ++i)
    {
        ht.insert (i, rand());
    }

    printf ( "size=%lu,hash_size=%d,threshold=%d\n", ht.size(), ht.hash_size(), ht.threshold());

    cout << show (ht) << endl;

    int i=0;
    for (LinearHashMap<int,int>::iterator it=ht.begin(); i<1000 &&it!=ht.end(); ++it,++i)
    {
        printf ("%d -> %d\n", it->key, it->value);
    }
}

    
// Brief: check LinearHashMap with __gnu_cxx::hash_map
// Author: gejun@baidu.com
TEST_F(test_usage_suite, bicheck_ohashmap_gnuhashmap)
{
    srand (time(0));
    
    hash_map<int,int> ref(10000);
    LinearHashMap<int,int> ht;
    ht.create(10000,50);

    for (int j=0; j<20; ++j) {
        ht.clear();
        ref.clear();
        for (int i=0; i<100000; ++i) {
            int k = rand()%2001;
            int p = rand () % 1000;
            if (p < 600) {
                ht.insert (k,i);
                ref[k] = i;
            } else if (p < 999) {
                ht.erase(k);
                ref.erase(k);
            } else {
                ht.clear();
                ref.clear();
            }
        }
        
        ASSERT_EQ (ht.size(), ref.size());

        // bi-check
        printf ("ht.size=%lu, ref.size=%lu\n", ht.size(), ref.size());
        for (LinearHashMap<int,int>::iterator it=ht.begin(); it!=ht.end(); ++it) {
            hash_map<int,int>::iterator it2 = ref.find(it->key);
            ASSERT_TRUE (it2 != ref.end());
            ASSERT_EQ (it2->second, it->value);
        }

        for (hash_map<int,int>::iterator it=ref.begin(); it!=ref.end(); ++it) {
            int* pv = ht.seek(it->first);
            ASSERT_TRUE (pv != NULL);
            ASSERT_EQ (*pv, it->second);
        }
    }
    
}

// Brief: check im_ohashset_t with std::set
// Author: gejun@baidu.com
/*
  TEST_F(test_usage_suite, hashset)
  {
  srand (time(0));

  std::set<int> ref;
  ExtHashSet<int> ht(10000);


  for (int j=0; j<20; ++j)
  {
  ht.clear();
  ref.clear();
  for (int i=0; i<100000; ++i)
  {
  int k = rand()%2001;
  if ((rand() % 100) < 90)
  {
  ht.insert (k);
  ref.insert (k);
  }
  else
  {
  ht.Remove (k);
  ref.erase(k);
  }
  }

  ASSERT_EQ (ht.size(), ref.size());

  // bi-check
  printf ("ht.size=%lu, ref.size=%lu\n", ht.size(), ref.size());
  for (ExtHashSet<int>::iterator it=ht.begin(); it!=ht.end(); ++it)
  {
  ASSERT_TRUE (ref.find(*it) != ref.end());
  }
  for (std::set<int>::iterator it=ref.begin(); it!=ref.end(); ++it)
  {
  ASSERT_TRUE (ht.find(*it) != ht.end());
  }
  }
  }

*/


