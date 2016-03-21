// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Test dyn_unique_index.h
// Author: gejun@baidu.com

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <ext/hash_map>
#include "st_timer.h"
#include "dyn_table.h"
#include "cow_table.hpp"

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

TEST_F(test_usage_suite, sanity)
{
    DynTable t1;
    ASSERT_TRUE(t1.not_init());
    ASSERT_EQ(0, t1.init("int32 apple, int16 orange, int16 pear"));
    ASSERT_EQ(0, t1.add_index("ST_UNIQUE_KEY(apple, orange, NBUCKET=1000, LOADFACTOR=70)"));
    ASSERT_EQ(1ul, t1.index_num());
    ASSERT_EQ(0, t1.add_index("ST_UNIQUE_KEY(apple)"));
    ASSERT_EQ(2ul, t1.index_num());
    ASSERT_EQ(ECONFLICT, t1.add_index("ST_UNIQUE_KEY(pear)"));
    ASSERT_FALSE(t1.not_init());
    ASSERT_EQ(2ul, t1.index_num());
    ASSERT_TRUE(t1.empty());
    ASSERT_EQ(0ul, t1.size());
    
    t1.insert(1, 1, 1);
    ASSERT_EQ(1ul, t1.size());
    ASSERT_FALSE(t1.empty());

    t1.insert(1, 10, 100);
    ASSERT_EQ(1ul, t1.size());

    t1.insert(2, 2, 2);
    ASSERT_EQ(2ul, t1.size());

    t1.insert(2, 20, 200);
    ASSERT_EQ(2ul, t1.size());

    t1.insert(3, 30, 300);
    ASSERT_EQ(3ul, t1.size());

    ASSERT_FALSE(t1.insert(1, 1));
    ASSERT_FALSE(t1.insert(1, 1, 1, 1));


    SCOPED_DYN_TUPLE(ref1, t1.schema());
    ref1.set_by_string("1 10 100", ' ');

    SCOPED_DYN_TUPLE(ref2, t1.schema());
    ref2.set_by_string("2, 20, 200");

    SCOPED_DYN_TUPLE(ref3, t1.schema());
    ref3.set_by_string("3, 30, 300");
    
    {
        DynTable::Seeker sk1 = t1.make_seeker("apple");

        DynNormalIterator it1 = sk1(1);
        ASSERT_TRUE(it1 && *it1 == ref1);    

        DynNormalIterator it2 = sk1.seek_by_string(" 2 ");
        ASSERT_TRUE(it2 && *it2 == ref2);    
        DynNormalIterator it22 = sk1(2);
        ASSERT_TRUE(it22 && *it22 == ref2);    

        DynNormalIterator it3 = sk1(3);
        ASSERT_TRUE(it3 && *it3 == ref3);

        ASSERT_FALSE(sk1(-1));
        ASSERT_FALSE(sk1(4));
        ASSERT_FALSE(sk1(5));
    }

    ASSERT_TRUE(NULL == t1.make_seeker("orange"));
    
    {
        DynTable::Seeker sk2 = t1.make_seeker("orange , apple");

        DynNormalIterator it1 = sk2(10, 1);
        ASSERT_TRUE(it1 && *it1 == ref1);    

        DynNormalIterator it2 = sk2(20, 2);
        ASSERT_TRUE(it2 && *it2 == ref2);    
        DynNormalIterator it22 = sk2.seek_by_string(" 20 , 2 ");
        ASSERT_TRUE(it22 && *it22 == ref2);
        
        DynNormalIterator it3 = sk2(30, 3);
        ASSERT_TRUE(it3 && *it3 == ref3);
        
        ASSERT_FALSE(sk2(-1));
        ASSERT_FALSE(sk2(4));
        ASSERT_FALSE(sk2(5));
        ASSERT_FALSE(sk2(1, 10));
        ASSERT_FALSE(sk2(10, 2));
        ASSERT_FALSE(sk2(2, 20));
        ASSERT_FALSE(sk2(3, 30));
    }

    for (DynNormalIterator it = t1.all(); it != NULL; ++it) {
        cout << *it << endl;
    }

    DynTable::Eraser es1 = t1.make_eraser("apple, orange");
    ASSERT_FALSE(es1(1, 11));
    ASSERT_TRUE(es1(1, 10));
    ASSERT_EQ(2ul, t1.size());
    ASSERT_TRUE(es1(2, 20));
    ASSERT_EQ(1ul, t1.size());

    ASSERT_TRUE(NULL == t1.make_eraser("orange"));

    DynTable::Eraser es2 = t1.make_eraser(" apple ");
    ASSERT_FALSE(es2(30, 3));
    ASSERT_FALSE(es2(30));
    ASSERT_TRUE(es2(3));
    ASSERT_EQ(0ul, t1.size());
    ASSERT_TRUE(t1.empty());
    
    cout << t1 << endl;
}

DEFINE_COLUMN(SITE_ID, int);
DEFINE_COLUMN(UNIT_ID, int);
DEFINE_COLUMN(BID, int);


TEST_F(test_usage_suite, perf_with_cow_table)
{
    typedef ST_TABLE(SITE_ID, UNIT_ID, BID, ST_UNIQUE_KEY(SITE_ID, UNIT_ID)) RefTable;

    srand(67);
    const size_t N = 100000;
    const double DEL_RATIO = 0;
    const double RANGE_RATIO = 0.1;
    NaiveTimer tm;
    DynTable t1, t3;
    RefTable t2;
    std::vector<int> rand_list;
    std::vector<std::string> str_list;
    ostringstream oss;


    // create t1
    ASSERT_EQ(0, t1.init("int site_id, int unit_id, int bid"));
    ASSERT_EQ(0, t1.add_index_format(
                  "ST_UNIQUE_KEY(site_id, unit_id, NBUCKET=%zu)", (size_t)(N*1.5)));
    ASSERT_EQ(1ul, t1.index_num());
    ASSERT_FALSE(t1.not_init());
    ASSERT_EQ(0ul, t1.size());
    ASSERT_TRUE(t1.empty());

    // create t2
    t2.init((size_t)(N*1.5));

    // create t3 which is exactly same with t1;
    t3 = t1;
        
    rand_list.reserve(3*N);
    for (size_t i = 0; i < N; ++i) {
        oss.str("");
        if (i > 0 && (rand() % 100) < (int)(DEL_RATIO * 100)) {
            rand_list.push_back(-rand_list[rand() % rand_list.size()]);
            oss << -rand_list.back();

            rand_list.push_back(rand() % (int)(N * RANGE_RATIO));
            oss << ", " << rand_list.back();

            rand_list.push_back(rand() % (int)(N * RANGE_RATIO));
        } else {
            rand_list.push_back(rand() % (int)(N * RANGE_RATIO));
            oss << rand_list.back();
            
            rand_list.push_back(rand() % (int)(N * RANGE_RATIO));
            oss << ", " << rand_list.back();

            rand_list.push_back(rand() % (int)(N * RANGE_RATIO));
            oss << ", " << rand_list.back();
        }
        str_list.push_back(oss.str());
    }

    {
        tm.start();
        for (size_t i = 0; i < N; ++i) {
            if (rand_list[3*i] >= 0) {
                t2.insert(rand_list[3*i], rand_list[3*i+1], rand_list[3*i+2]);
            } else {
                t2.erase<SITE_ID, UNIT_ID>(-rand_list[3*i], rand_list[3*i+1]);
            }
        }
        tm.stop();
        const double ae1 = tm.n_elapsed()/(double)N;
        cout << "CowTable.RandomInsertErase: " << ae1 << "ns" << endl;


        tm.start();
        DynTable::Eraser es = t1.make_eraser("site_id, unit_id");
        for (size_t i = 0; i < N; ++i) {
            if (rand_list[3*i] >= 0) {
                t1.insert(rand_list[3*i], rand_list[3*i+1], rand_list[3*i+2]);
            } else {
                es(-rand_list[3*i], rand_list[3*i+1]);
            }
        }
        tm.stop();
        const double ae2 = tm.n_elapsed()/(double)N;
        cout << "DynTable.RandomInsertErase: " << ae2 << "ns (" << ae2/ae1 << ')' << endl;


        tm.start();
        DynTable::Eraser es2 = t3.make_eraser("site_id, unit_id");
        for (size_t i = 0; i < N; ++i) {
            if (rand_list[3*i] >= 0) {
                t3.insert_by_string(str_list[i].c_str());
            } else {
                es2.erase_by_string(str_list[i].c_str());
            }
        }
        tm.stop();
        const double ae3 = tm.n_elapsed()/(double)N;
        cout << "DynTable2.RandomInsertErase: " << ae3 << "ns (" << ae3/ae1 << ')' << endl;
    }

    ASSERT_EQ(t3.size(), t1.size());
    ASSERT_EQ(t2.size(), t1.size());
    cout << "Size: " << t1.size() << endl;

    {
        int sum1 = 0, sum2 = 0, sum3 = 0;
        std::vector<int> seek_list;
        std::vector<std::string> seek_str_list;
        for (size_t i = 0; i < N; ++i) {
            if (rand_list[3*i] >= 0 && ((rand() % 100) < 70)) {
                oss.str("");
                
                seek_list.push_back(rand_list[3*i]);
                oss << seek_list.back();
                
                seek_list.push_back(rand_list[3*i+1]);
                oss << ", " << seek_list.back();

                seek_str_list.push_back(oss.str());
            }
        }
        //std::random_shuffle(seek_list.begin(), seek_list.end());
        const size_t SN = seek_list.size() / 2;
    
        tm.start();
        for (size_t i = 0; i < SN; ++i) {
            RefTable::SeekIterator<SITE_ID, UNIT_ID>::R it =
                t2.seek<SITE_ID, UNIT_ID>(seek_list[2*i], seek_list[2*i+1]);
            sum1 += it ? it->at<BID>() : 0;
        }
        tm.stop();
        const double ae1 = tm.n_elapsed()/(double)(SN ? SN : 1);
        cout << "CowTable.RandomSeekExist: " << ae1 << "ns" << endl;

        tm.start();
        DynTable::Seeker sk = t1.make_seeker("site_id, unit_id");
        for (size_t i = 0; i < SN; ++i) {
            //DynNormalIterator it = sk(seek_list[2*i], seek_list[2*i+1]);
            SCOPED_CALL_SEEKER(it, sk, seek_list[2*i], seek_list[2*i+1]);
            sum2 += it ? it->at("bid").to_int32() : 0;
        }
        tm.stop();
        const double ae2 = tm.n_elapsed()/(double)(SN ? SN : 1);
        cout << "DynTable.RandomSeekExist: " << ae2 << "ns (" << ae2/ae1 << ")" << endl;

        tm.start();
        DynTable::Seeker sk2 = t1.make_seeker("site_id, unit_id");
        for (size_t i = 0; i < SN; ++i) {
            DynNormalIterator it = sk2.seek_by_string(seek_str_list[i].c_str());
            sum3 += it ? it->at("bid").to_int32() : 0;
        }
        tm.stop();
        const double ae3 = tm.n_elapsed()/(double)(SN ? SN : 1);
        cout << "DynTable2.RandomSeekExist: " << ae3 << "ns (" << ae3/ae1 << ")" << endl;
        
        ASSERT_EQ(sum1, sum2);
        ASSERT_EQ(sum1, sum3);
        cout << "Seeked: " << SN << endl;
    }

    cout << "DynTable: " << t1 << endl
         << "CowTable: " << show(t2) << endl
         << "DynTable2: " << t3 << endl;
    
    {
        TIME_SCOPE_IN_MS("Verify DynTable/CowTable is mutually included:");
        for (DynNormalIterator it = t1.all(); it; ++it) {
            RefTable::SeekIterator<SITE_ID, UNIT_ID>::R it2 =
                t2.seek<SITE_ID, UNIT_ID>(it->at("site_id").to_int32(),
                                          it->at("unit_id").to_int32());
            ASSERT_TRUE(it2);
            ASSERT_EQ(it->at("bid").to_int32(), it2->at<BID>());
        }

        DynTable::Seeker sk1 = t1.make_seeker("site_id, unit_id");
        for (RefTable::Iterator it = t2.begin(); it != t2.end(); ++it) {
            DynNormalIterator it2 = sk1(it->at<SITE_ID>(), it->at<UNIT_ID>());
            ASSERT_TRUE(it2);
            ASSERT_EQ(it2->at("site_id").to_int32(), it->at<SITE_ID>());
            ASSERT_EQ(it2->at("unit_id").to_int32(), it->at<UNIT_ID>());
            ASSERT_EQ(it2->at("bid").to_int32(), it->at<BID>());
        }
    
    }
}


