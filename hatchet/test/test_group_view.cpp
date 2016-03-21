// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: jiangrujie@baidu.com
// Date: 2011-02-24 14:08

#include <gtest/gtest.h>
#include "group_view.hpp"
#include "st_timer.h"
#include "c_connector.hpp"
#include <ext/hash_map>
#include <set>

using namespace st;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

struct least_two_items {
    template<class _Cluster>
    bool operator()(const _Cluster *p_clu) const
    { return (p_clu->size() > 1); }
};

DEFINE_COLUMN (UNIT_ID, u_int);
DEFINE_COLUMN (PLAN_ID, u_int);
DEFINE_COLUMN (USER_ID, u_int);
DEFINE_COLUMN (STATUS, u_int);
DEFINE_COLUMN (SITE_ID, u_int);
DEFINE_COLUMN (TRADE_ID, u_int);
DEFINE_COLUMN (SIGN1, u_int);
DEFINE_COLUMN (SIGN2, u_long);
DEFINE_COLUMN (IDEA, u_int);
DEFINE_COLUMN (KEYWORD, u_int);
DEFINE_COLUMN (TPC, u_int);
DEFINE_COLUMN (TERM, u_int);
DEFINE_COLUMN (ADQ, u_int);
DEFINE_COLUMN (BID, u_int);

class test_tb1_suite : public ::testing::Test{
protected:
    typedef ST_TABLE(UNIT_ID, PLAN_ID, USER_ID, STATUS,
                     ST_UNIQUE_KEY(USER_ID, ST_CLUSTER_KEY(UNIT_ID)),
                     ST_UNIQUE_KEY(UNIT_ID),
                     ST_UNIQUE_KEY(PLAN_ID, ST_CLUSTER_KEY(UNIT_ID))) T1;

    typedef ST_TABLE(USER_ID, STATUS, ST_UNIQUE_KEY(USER_ID)) T2;

    typedef GroupView<T1, 0> G1;
    
    typedef std::set<u_int> ref_t;
    
    test_tb1_suite(){};
    virtual ~test_tb1_suite(){};
    virtual void SetUp() {
        ASSERT_TRUE (T1::N_INDEX == 3);

        srand(time(0));
        tb1.init(1000, 70);
        tb2.init(1000, 70);
    };
    virtual void TearDown() {
    };

    T1 tb1;
    T2 tb2;
};

template <class _Tup>
class TrivialObserver : public BaseObserver<const _Tup*> {
public:
    TrivialObserver() : p_tup(NULL), cnt(0)
    {}
    
    void on_event (const _Tup* tup)
    {
        p_tup = tup;
        ++ cnt;
    }

    const _Tup* p_tup;
    int cnt;
};
    
TEST_F(test_tb1_suite, group_view_sanity)
{
    G1 v1(&tb1);
    ASSERT_TRUE (v1.empty());
    
    tb1.insert (1, 10, 100, 9);
    tb1.insert (2, 20, 100, 9);
    ASSERT_EQ (v1.size(), 1ul);
    
    tb1.erase<USER_ID>(100);
    ASSERT_TRUE (v1.empty());

    tb1.insert (1, 10, 100, 9);
    tb1.insert (2, 20, 100, 9);
    tb1.insert (3, 30, 200, 9);
    tb1.insert (4, 40, 200, 9);

    int count = 0;
    for(G1::Iterator it = v1.begin(), it_e = v1.end(); 
            it != it_e; ++it) {
        ASSERT_EQ (it->at<G1::CLUSTER_ATTR>()->size(), 2ul);
        count++;
    }
    ASSERT_EQ (count, 2);

    tb1.clear();
    ASSERT_TRUE (v1.empty());
}

TEST_F(test_tb1_suite, group_view_selector)
{
    typedef ST_SELECTOR(
        ST_FROM(T2, G1), 
        ST_WHERE(eq(TBL1<USER_ID>, TBL2<USER_ID>),
                 least_two_items(TBL2<G1::CLUSTER_ATTR>))) Sel;

    for(int i = 0; i < 200; i++) {
        tb1.insert(rand() % 200, rand() % 100, 
                rand() % 70, rand() % 100);
        tb2.insert(rand() % 70, rand() % 100); 
    }
    
    G1 v1(&tb1);
    Sel s(&tb2, &v1);
    ref_t results;
    
    for(T2::Iterator it1 = tb2.begin(); it1; ++it1) {
        for(G1::Iterator it2 = v1.begin(); it2; ++it2) {
            if(it1->at<USER_ID>() == it2->at<USER_ID>()
                    && it2->at<G1::CLUSTER_ATTR>()->size() > 1) {
                results.insert(it1->at<USER_ID>());
            }
        }
    }
    size_t size = 0;
    for(Sel::Iterator it = s.select(); it; ++it) {
        int user_id = it.at<TBL1, USER_ID>();
        ASSERT_TRUE(results.find(user_id) != results.end());
        size++;
    }

    ASSERT_EQ(size, results.size());
    cout << size << "tuples returned" << endl;

    tb1.clear();
    tb2.clear();
}

// This case won't pass.
#if 0
TEST_F(test_tb1_suite, group_view_connector)
{
    typedef ST_TABLE(USER_ID, ST_UNIQUE_KEY(USER_ID)) DestTbl;
    
    typedef ST_CONNECTOR(
        DestTbl, 
        ST_PICK(TBL1<USER_ID>), 
        ST_FROM(G1, T2), 
        ST_WHERE(eq(TBL1<USER_ID>, TBL2<USER_ID>),
                 least_two_items(TBL1<G1::CLUSTER_ATTR>))) Trig1;

    DestTbl dest;
    dest.init(1000, 80);
    
    G1 v1(&tb1);
    Trig1 trig1(&dest, &v1, &tb2);
    ref_t results;
    
    for(int i = 0; i < 100; i++) {
        tb1.insert(rand() % 200, rand() % 100, 
                rand() % 70, rand() % 100);
        tb2.insert(rand() % 70, rand() % 100); 
    }
    trig1.connect();
    
    for(int i = 0; i < 5; i++) {
        for(T2::iterator it1 = tb2.begin(); it1; ++it1) {
            for(G1::iterator it2 = v1.begin(); it2; ++it2) {
                if(it1->at<USER_ID>() == it2->at<USER_ID>()
                        && it2->at<G1::CLUSTER_ATTR>()->size() > 1) {
                    results.insert(it1->at<USER_ID>());
                }
            }
        }
        
        size_t size = 0;
        int user_id;
        size_t res_size = results.size();
        CAP(T1::seek_iterator, USER_ID) itt;
        for(DestTbl::iterator it = dest.begin(); it; ++it) {
            int user_id = it->at<USER_ID>();
            ASSERT_TRUE(results.find(user_id) != results.end());
            results.erase(user_id);
            size++;
        }
        ASSERT_EQ(size, res_size) << (user_id = *(results.begin()), user_id) << endl
             << (itt = tb1.seek<USER_ID>(user_id), ++itt, ++itt, itt) << endl;
        cout << size << "tuples returned" << endl;
       

        for(int j = 0; j < 10; j++) {
            int a = rand() % 70;
            if(rand() % 100 >= 40) {
                tb1.insert(rand() % 200, rand() % 100, 
                        a, rand() % 100);
                cout << a << endl;
            } else {
                tb1.erase<UNIT_ID>(rand() % 200);
                tb1.erase<UNIT_ID>(rand() % 200);
            }
        }
        results.clear();
    }

    tb1.clear();
    tb2.clear();
}
#endif
