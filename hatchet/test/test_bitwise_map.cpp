// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Test st_hash_map.hpp
// Author: gejun@baidu.com

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include "bitwise_map.hpp"
#include "cow_hash_map.hpp"
#include "st_errno.h"
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


TEST_F(test_usage_suite, init_test)
{
    typedef BitwiseMap<int,long> Map;

    //a small size init, use default length value
    Map m;
    ASSERT_TRUE(m.not_init());
    ASSERT_TRUE(0 == m.init(128)); 
    ASSERT_FALSE(m.not_init());
    EXPECT_EQ(CH_MIN_BITMAP_LEN, m.bitmap_len());
    EXPECT_EQ(m.size(), 0ul);
    EXPECT_TRUE(m.empty());
 

    //bitmap len should not be too large
    Map m2;
    ASSERT_TRUE(m2.not_init());
    ASSERT_TRUE(ECONFLICT == m2.init(90000000)); 
    EXPECT_TRUE(m2.not_init());

    //normal length    
    Map m3;
    ASSERT_TRUE(m3.not_init());
    ASSERT_TRUE(0 == m3.init(1024)); 
    ASSERT_FALSE(m3.not_init());
    EXPECT_EQ(1024ul, m3.bitmap_len());
    EXPECT_EQ(m3.size(), 0ul);
    EXPECT_TRUE(m3.empty());
 
}


TEST_F(test_usage_suite, sanity)
{
    typedef BitwiseMap<int,long> Map;
    Map m;

    ASSERT_TRUE(m.not_init());
    m.init(1024);
    ASSERT_FALSE(m.not_init());
    EXPECT_EQ(1024ul, m.bitmap_len());
    EXPECT_EQ(m.size(), 0ul);
    EXPECT_TRUE(m.empty());
   
    //insert an item
    Map::Pointer p1 = m.insert(1,10);
    EXPECT_EQ(m.size(),1ul);
    EXPECT_FALSE(m.empty());
    EXPECT_TRUE(p1 && ((*p1) == 10));

    //seek an existing item
    Map::Pointer p2 = m.seek(1);
    EXPECT_TRUE(p2 && ( (*p2) == 10));
    
    //seek a non-existing item
    p2 = m.seek(0);
    EXPECT_TRUE(NULL == p2);
    
    //inset another item
    p1 = m.insert(24,15);
    EXPECT_EQ(m.size(),2ul);
    EXPECT_FALSE(m.empty());
    EXPECT_TRUE(p1 && ((*p1) == 15));
    p2 = m.seek(24);
    EXPECT_TRUE(p2 && ( (*p2) == 15));
    
    //inset a third item
    p1 = m.insert(1024,66);
    EXPECT_EQ(m.size(),3ul);
    EXPECT_TRUE(p1 && ((*p1) == 66));
    p2 = m.seek(1024);
    EXPECT_TRUE(p2 && ( (*p2) == 66));

    //override an item
    p1 = m.insert(24,99);
    EXPECT_EQ(m.size(),3ul);
    EXPECT_TRUE(p1 && ((*p1) == 99));
    p2 = m.seek(24);
    EXPECT_TRUE(p2 && ( (*p2) == 99));

    //erase an item
    ASSERT_TRUE(true == m.erase(24));
    EXPECT_EQ(m.size(),2ul);
    p2 = m.seek(24);
    EXPECT_TRUE(NULL == p2);

    //erase an index out of boundary item
    ASSERT_TRUE(false == m.erase(5000));
    EXPECT_EQ(m.size(),2ul);

    //erase an non-existing item
    ASSERT_TRUE(false == m.erase(26));
    EXPECT_EQ(m.size(),2ul);

    //clear items
    m.clear();
    EXPECT_EQ(m.size(),0ul);
    Map::BitmapType bm = m.get_bitmap();
    ASSERT_FALSE( NULL == bm);
    EXPECT_TRUE(1 == bitmap_empty(bm,1025));
    EXPECT_EQ(m.size(),0ul);

}

TEST_F(test_usage_suite, resize_test)
{
    typedef BitwiseMap<int,long> Map;
    Map m;

    ASSERT_TRUE(0 == m.init(512));

    ASSERT_TRUE(true == m.resize(1024));
    EXPECT_FALSE(m.not_init());
    EXPECT_EQ(m.size(),0ul);

    m.insert(60,24);
    EXPECT_EQ(m.size(),1ul);
    EXPECT_EQ(m.bitmap_len(),1024ul);

    //resize to a smaller size
    EXPECT_FALSE(m.resize(256));

    //resize to a larger size 
    ASSERT_TRUE(m.resize(2048));
    EXPECT_EQ(m.size(),1ul);
    EXPECT_EQ(m.bitmap_len(),2048ul);

    //after resize, we can still find (60,24)    
    Map::Pointer p2 = m.seek(60);
    EXPECT_TRUE(p2 && ( (*p2) == 24));
 
    //insert a large key,value pair
    m.insert(4095,50); 
    EXPECT_EQ(m.size(),2ul);
    //seek items    
    p2 = m.seek(60);
    EXPECT_TRUE(p2 && ( (*p2) == 24));
    p2 = m.seek(4095);
    EXPECT_TRUE(p2 && ( (*p2) == 50));
    
}

TEST_F(test_usage_suite, copy_and_assign)
{
    typedef BitwiseMap<int,long> Map;
    Map m;

    ASSERT_TRUE(m.not_init());
    m.init(1024);
    ASSERT_FALSE(m.not_init());
    EXPECT_EQ(m.bitmap_len(),1024ul);
    EXPECT_EQ(m.size(), 0ul);
    EXPECT_TRUE(m.empty());
    
    //insert an item
    Map::Pointer p1 = m.insert(38,10);
    EXPECT_EQ(m.size(),1ul);
    EXPECT_FALSE(m.empty());
    EXPECT_EQ(m.bitmap_len(),1024ul);
    EXPECT_TRUE(p1 && ((*p1) == 10));

    
    //copy a map
    Map m2(m);
    ASSERT_FALSE(m2.not_init());
    EXPECT_EQ(m2.size(),1ul);
    EXPECT_FALSE(m2.empty());
    EXPECT_EQ(m2.bitmap_len(),1024ul);
    Map::Pointer p2 = m2.seek(38); 
    EXPECT_TRUE(p2 && (*p2) == 10);
    
    //copy an uninitialized map
    Map m3;
    ASSERT_TRUE(m3.not_init());
    Map m4(m3);
    EXPECT_TRUE(m4.not_init()); 

    //assign an initialized map
    Map m5;
    m5 = m; 
    ASSERT_FALSE(m5.not_init());
    EXPECT_EQ(m5.size(),1ul);
    EXPECT_FALSE(m5.empty());
    EXPECT_EQ(m5.bitmap_len(),1024ul);
    p2 = m5.seek(38); 
    EXPECT_TRUE(p2 && (*p2) == 10);
 
    //asign an uninitialized map to an uninitialized map
    m4 = m3;
    ASSERT_TRUE(m4.not_init());

    //asign an uninitialized map to an uninitialized map
    m5 = m3;
    ASSERT_TRUE(m5.not_init());

}


TEST_F(test_usage_suite, random_insert_earse_test)
{
    srand (time(0));

    const size_t initial_bucket_num  = 5000000;
    const size_t n_value = 5000000;
    const size_t k_range = 3000000; 

    CowHashMap<int, u_long> cow2_ht;
    cow2_ht.init(initial_bucket_num);

    BitwiseMap<int,u_long> st_bm;
    st_bm.init(initial_bucket_num);
    
    std::vector<std::pair<int, u_long> > values;

    {
        TIME_SCOPE_IN_MS("Prepare_random_values: ");
                    
        for (size_t i=0; i<n_value; ++i) {
            int k = rand() % k_range + 1;
            int v = rand();
            values.push_back (std::pair<int,u_long>(k, v));
        }
    }
        
    {
        TIME_SCOPE_IN_NS("random insert_cow_hash_map2: ", values.size());
        for (size_t i=0; i<values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];
            cow2_ht.insert (p.first, p.second);
        }
    }
    
    {
        TIME_SCOPE_IN_NS("random insert_st_bm: ", values.size());    
        for (size_t i=0; i<values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];
            st_bm.insert (p.first, p.second);
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
                st_bm.erase(r);
            } else {
                int k = rand() % k_range;
                int v = rand();
                cow2_ht.insert(k, v);
                st_bm.insert(k, v);
            }
        }
    }
    
    cout << " cow2:" << show(cow2_ht) << endl
         << " bm :"  << show(st_bm) << endl
         << "}" << endl
        ;
    
    std::random_shuffle (values.begin(), values.end());

    int v;
    u_long *p;    

    {
        TIME_SCOPE_IN_NS_NO_NL("seek_cow_hash_map2_random: ", values.size());
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
        TIME_SCOPE_IN_NS_NO_NL("seek st_bm_random: ", values.size());
        v = 0;
        for (size_t i=0; i<values.size(); ++i) {
            p = st_bm.seek (values[i].first);
            if (p) {
                v += *p;
            }
        }
    }
    cout << ", v=" << v << endl;

    
}

TEST_F(test_usage_suite, nonrandom_insert_erase)
{
    srand (time(0));

    const size_t initial_bucket_num  = 5000000;
    const size_t n_value = 3000000;

    CowHashMap<int, u_long> cow2_ht;
    cow2_ht.init(initial_bucket_num);

    BitwiseMap<int,u_long> st_bm;
    st_bm.init(initial_bucket_num);
    
    std::vector<std::pair<int, u_long> > values;

    {
        TIME_SCOPE_IN_MS("Prepare_non_random_values: ");
                    
        for (size_t i=0; i<n_value; ++i) {
            int k = i;
            int v = rand();
            values.push_back (std::pair<int,u_long>(k, v));
        }
    }
        
    {
        TIME_SCOPE_IN_NS("nonrandom insert_cow_hash_map2: ", values.size());
        for (size_t i=0; i<values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];
            cow2_ht.insert (p.first, p.second);
        }
    }
    
    {
        TIME_SCOPE_IN_NS("nonrandom insert_st_bm: ", values.size());    
        for (size_t i=0; i<values.size(); ++i) {
            std::pair<int, u_long>& p = values[i];
            st_bm.insert (p.first, p.second);
        }
    }

    std::vector<int> seq;
    {
        TIME_SCOPE_IN_MS("Prepare_nonrandom_seek_sequence: ");    
        seq.reserve (values.size());
        for (size_t i=0; i<values.size(); ++i) {
            seq.push_back (i);
        }
        std::random_shuffle (seq.begin(), seq.end());
    }


    int v;
    u_long *p;    

    {
        TIME_SCOPE_IN_NS_NO_NL("nonrandom_test,seek_cow_hash_map2: ", values.size());
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
        TIME_SCOPE_IN_NS_NO_NL("nonrandom_test,seek st_bm: ", values.size());
        v = 0;
        for (size_t i=0; i<values.size(); ++i) {
            p = st_bm.seek (values[i].first);
            if (p) {
                v += *p;
            }
        }
    }
    cout << ", v=" << v << endl;
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

    int get_x() {return x_;}

    bool operator== (const Value& rhs) const { return x_ == rhs.x_; }
    bool operator!= (const Value& rhs) const { return x_ != rhs.x_; }

friend ostream& operator<< (ostream& os, const Value& v)
    { return os << v.x_; }

    int x_;
};


TEST_F(test_usage_suite, iterator_test)
{
    srand (0);

    CowHashMap<int, Value> ref;
    BitwiseMap<int, Value> bm;
    
    const int init_bucket_num = 5000000;
    const int n_value         = 5000000;
    bm.init (init_bucket_num);
    ref.init (init_bucket_num, 80);
  
    const int k_range = 200000;

    for (int i = 0; i < n_value; ++i) {
        int k = rand() % k_range;
        int p = rand() % 1000;
        if (p < 600) {
            bm.insert(k, i);
            ref.insert(k,i);
        } else if(p < 999) {
            if( true == bm.erase (k) 
             && true == ref.erase (k)) {
            }
        }
    }

    size_t iter_num = 0;
    for (BitwiseMap<int, Value>::Iterator it = bm.begin();
                 it != bm.end(); ++it)
    {
        CowHashMap<int,Value>::Pointer it2 = ref.seek(it->first);
        EXPECT_EQ (*it2, it->second);
        iter_num ++;
    }
    
    EXPECT_EQ (bm.size(), ref.size());
    EXPECT_EQ (iter_num, ref.size());
   
    BitwiseMap<int,Value>::Iterator it = bm.begin();
    it.set_end();

    if(it != bm.end()) {
        cout << "set_end failed" << endl;
    }
    
    long v;
    {
        v = 0;
        TIME_SCOPE_IN_NS_NO_NL("iter cow_hashmap: ", ref.size());
        
        for (CowHashMap<int, Value>::Iterator it = ref.begin();
                     it != ref.end(); ++it)
        {
            v += (it->second).get_x();
        }
    }

    cout << ", v = " << v << endl; 
    
     {
        TIME_SCOPE_IN_NS_NO_NL("iter st_bitmap: ", bm.size());
        
        v = 0;
        for (BitwiseMap<int, Value>::Iterator it = bm.begin();
                     it != bm.end(); ++it)
        {
            v += (it->second).get_x();
        }
    }

    cout << ", v = " << v << endl; 
 
    bm.clear ();
    ref.clear ();

}

