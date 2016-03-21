// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Test cow_hash_cluster_map.hpp
// Author: gejun@baidu.com
// Date: 2011-01-05 09:13 

#include <gtest/gtest.h>
#include <cow_hash_cluster_map.hpp>
#include <map>
#include <st_timer.h>
#include <compare.hpp>

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

TEST_F(test_usage_suite, sanity)
{
    typedef CowHashClusterMap<int, short, long> Map;
    Map m;

    ASSERT_TRUE(m.not_init());
    m.init(1000, 70);
    ASSERT_FALSE(m.not_init());
    ASSERT_EQ(m.size(), 0ul);
    ASSERT_TRUE(m.empty());

    // Initial insertion
    m.insert(1, 10, 100);
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    Map::Pointer p = m.seek(1, 10);
    ASSERT_TRUE(p && *p == 100);

    // Override
    m.insert(1, 10, 1000);
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    p = m.seek(1, 10);
    ASSERT_TRUE(p && *p == 1000);

    // Insert another in the cluster
    m.insert(1, 11, 1001);
    ASSERT_EQ(m.size(), 2ul);
    ASSERT_FALSE(m.empty());
    p = m.seek(1, 11);
    ASSERT_TRUE(p && *p == 1001);
    
    // Insert anthter cluster
    m.insert(2, 20, 200);
    ASSERT_EQ(3ul, m.size());
    ASSERT_FALSE(m.empty());
    p = m.seek(2, 20);
    ASSERT_TRUE(p && *p == 200);

    // Insert anthter in the cluster
    m.insert(2, 19, 200);
    ASSERT_EQ(4ul, m.size());
    ASSERT_FALSE(m.empty());
    p = m.seek(2, 19);
    ASSERT_TRUE(p && *p == 200);
    
    // Erase cluster
    ASSERT_EQ(2ul, m.erase(1));
    ASSERT_EQ(m.size(), 2ul);
    ASSERT_FALSE(m.empty());
    ASSERT_FALSE(m.seek(1));

    // Erase one item in cluster
    ASSERT_TRUE(m.erase(2, 20));
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    ASSERT_FALSE(m.seek(2, 20));
    
    // Clear
    m.clear();
    ASSERT_EQ(m.size(), 0ul);
    ASSERT_TRUE(m.empty());
    ASSERT_FALSE(m.seek(1));
    ASSERT_FALSE(m.seek(2));
}

struct Key {
    Key () : h(0), c(0) {}
    Key (int h2, int c2) : h(h2), c(c2) {}

    union {
        struct {
            int h;
            int c;
        };
        long p;
    };

    bool operator< (const Key& other) const { return p < other.p; }
    bool operator== (const Key& other) const { return p == other.p; }
};

TEST_F(test_usage_suite, basic_insert_erase)
{
    srand(time(0));
    //srand(0);

    typedef CowHashClusterMap<int, int, int> HC;
    C_ASSERT_SAME(HC::HKey, int, bad);
    C_ASSERT_SAME(HC::CKey, int, bad);
    C_ASSERT_SAME(HC::CVal, int, bad);
    cout << "Item: " << c_show(HC::Item) << endl
         << "HVal: " << c_show(HC::HVal) << endl
         << "Cluster: " << c_show(HC::Cluster) << endl;

    typedef std::map<Key, int> Ref;
    const int MAX_VALUE = 10000;
    
    HC hc, hc2;
    Ref ref;
    Key k;
    int v;

    ASSERT_EQ(0, hc.init(1024, 80));
    cout << "hc=" << show (hc).c_str() << endl;
    cout << "hc2=" << show (hc2).c_str() << endl;
    cout << "sizeof(HC::iterator)=" << sizeof(HC::Iterator) << endl;

    for (int j=0; j<100; ++j) {
        for (int i=0; i<MAX_VALUE; ++i) {
            k.h = rand()%MAX_VALUE;
            k.c = rand()%MAX_VALUE;
            if ((rand()%100) < 50) {
                v = rand () % 1000;
                hc.insert (k.h, k.c, v);
                ref[k] = v;
            } else {
                hc.erase (k.h, k.c);
                ref.erase (k);
            }
        }

        cout << "hc2=" << show(hc2).c_str() << endl;
        hc2 = hc;
        cout << "hc=" << show(hc).c_str() << endl;
    }

    Ref ref2;
    for (HC::Iterator it=hc.begin(), it_e=hc.end(); it!=it_e; ++it) {
        ref2[Key(it.key(), it.val().first)] = it.val().second;
        //ref2[Key((*it).first, (*it).second)] = (*it).third;
    }
    ASSERT_TRUE (ref == ref2);

    const HC::HMap *p_hm = hc.hash_map_ptr();
    std::set<int> hkey_set;
    for (HC::HMap::Iterator it=p_hm->begin(), it_e=p_hm->end()
             ; it!=it_e; ++it) {
        ASSERT_TRUE(hkey_set.find(it->hkey_) == hkey_set.end()) << it->hkey_;
        hkey_set.insert (it->hkey_);

        HC::Cluster* p_clu = &(it->cluster_);
        int last_ckey = INT_MIN;
        int cb_len = 0;
        for (HC::Cluster::Iterator it2=p_clu->begin(), it2_e=p_clu->end();
             it2!=it2_e; ++it2, ++cb_len) {
            ASSERT_TRUE(last_ckey < it2->first);
            last_ckey = it2->first;
        }
        ASSERT_TRUE (cb_len > 0);
    }

    HC::Pointer pt = hc.seek(1,1);
    if (pt != NULL) {
        cout << *pt << endl;
    }

    hc.clear();
    hc2.clear();

    ASSERT_EQ (hc.map_alloc()->alloc_num(), 0ul);
    ASSERT_EQ (hc.leaf_alloc()->alloc_num(), 0ul);
    ASSERT_EQ (hc.branch_alloc()->alloc_num(), 0ul);
}

