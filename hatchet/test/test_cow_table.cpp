// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2011-01-14 06:17

#include <gtest/gtest.h>
#include "cow_table.hpp"
#include "st_timer.h"
#include <ext/hash_map>
#include "unique_index.hpp"
#include "unique_cluster_index.hpp"

using namespace st;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

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
DEFINE_COLUMN (LITERATURE, std::string);

typedef ST_TABLE(
    UNIT_ID, PLAN_ID, USER_ID, STATUS,
    ST_UNIQUE_KEY(USER_ID, ST_REVERSED_CLUSTER_KEY(UNIT_ID)),
    ST_UNIQUE_KEY(UNIT_ID),
    ST_UNIQUE_KEY(PLAN_ID, ST_CLUSTER_KEY(UNIT_ID, MaxFanout<32>))) Table1;

class test_basic_suite : public ::testing::Test{
protected:
    test_basic_suite(){};
    virtual ~test_basic_suite(){};
    virtual void SetUp() {
        srand(time(0));
    };
    virtual void TearDown() {
    };
    NaiveTimer tm;
};

TEST_F(test_basic_suite, unique_index)
{
    typedef NamedTuple<ST_MAKE_LIST(UNIT_ID, PLAN_ID, USER_ID)> Tup;
    typedef UniqueIndex<Tup, Cons<UNIT_ID, void> > UI;
    typedef UniqueClusterIndex<Tup, Cons<PLAN_ID, void>, Cons<UNIT_ID, void>, 32, false> UCI;

    UI ui;
    ui.init(1000, 80);
    
    UCI uci;
    uci.init(1000, 80);

    ui.insert(Tup(1, 1, 1));
    ui.insert(Tup(1, 10, 10));
    ui.insert(Tup(2, 2, 2));
    ui.insert(Tup(3, 3, 3));
    cout << "UI:" << endl;
    for (UI::Iterator it = ui.begin(); it != ui.end(); ++it) {
        cout << *it << endl;
    }

    uci.insert(Tup(1, 1, 1));
    uci.insert(Tup(1, 10, 10));
    uci.insert(Tup(2, 2, 2));
    uci.insert(Tup(3, 3, 3));
    cout << "UCI:" << endl;
    for (UCI::Iterator it = uci.begin(); it != uci.end(); ++it) {
        cout << *it << endl;
    }
}

TEST_F(test_basic_suite, insert_by_file)
{
    // Prepare the file
    system("rm -f insert_by_file.tmp");
    system("echo -e '1\t10\t100' >> insert_by_file.tmp");
    system("echo -e '1\t20\t300\t100' >> insert_by_file.tmp");      // one more column
    system("echo -e '2\t20\t200' >> insert_by_file.tmp");
    system("echo -e '2\t20\t2a0' >> insert_by_file.tmp");           // Invalid character
    system("echo -e '3\t30\t300' >> insert_by_file.tmp");
    system("echo -e '4\t40' >> insert_by_file.tmp");                // less column
    system("echo -e '5\t4294967296\t500' >> insert_by_file.tmp");   // overflow
    system("echo -e '6\t4294967295\t600' >> insert_by_file.tmp");
    system("echo -e '' >> insert_by_file.tmp");                     // empty line
    system("echo -e '7\t-1\t700' >> insert_by_file.tmp");           // underflow
    
    typedef ST_TABLE(UNIT_ID, BID, SIGN2, ST_UNIQUE_KEY(UNIT_ID)) T;
    typedef std::set<T::Tup> S;
    T t1;
    ASSERT_EQ(0 ,t1.init(1000, 80));
    ASSERT_EQ(1, t1.insert_by_file("insert_by_file.tmp", '\t'));
    system("rm -f insert_by_file.tmp");
    
    S ref, s;
    ref.insert(T::Tup(1, 10, 100));
    ref.insert(T::Tup(2, 20, 200));
    ref.insert(T::Tup(3, 30, 300));
    ref.insert(T::Tup(6, 4294967295, 600));

    for (T::Iterator it = t1.begin(); it != t1.end(); ++it) {
        s.insert(*it);
    }
    ASSERT_TRUE(ref == s);
}

TEST_F(test_basic_suite, insert_by_file_and_check_max_lines)
{
    // Prepare the file
    system("rm -f insert_by_file.tmp");
    system("echo -e '1\t10\t100' >> insert_by_file.tmp");
    system("echo -e '2\t20\t200' >> insert_by_file.tmp");

    
    typedef ST_TABLE(UNIT_ID, BID, SIGN2, ST_UNIQUE_KEY(UNIT_ID)) T;
    typedef std::set<T::Tup> S;
    T t1;
    ASSERT_EQ(0 ,t1.init(1000, 80));
    ASSERT_EQ(0, t1.insert_by_file("insert_by_file.tmp", '\t', false, 1));
    system("rm -f insert_by_file.tmp");
    
    S ref, s;
    ref.insert(T::Tup(1, 10, 100));

    for (T::Iterator it = t1.begin(); it != t1.end(); ++it) {
        s.insert(*it);
    }
    ASSERT_TRUE(ref == s);
}

TEST_F(test_basic_suite, insert_by_file_and_support_string_type)
{
    // Prepare the file
    unlink("insert_by_file.tmp");
    system("echo -e '1\tabc' >> insert_by_file.tmp");
    system("echo -e '2\tdef' >> insert_by_file.tmp");

    typedef ST_TABLE(UNIT_ID, LITERATURE, ST_UNIQUE_KEY(UNIT_ID)) T;
    typedef std::set<T::Tup> S;
    T t1;
    ASSERT_EQ(0 ,t1.init(1000, 80));
    ASSERT_EQ(0, t1.insert_by_file("insert_by_file.tmp", '\t', false));
    unlink("insert_by_file.tmp");
    
    S ref, s;
    ref.insert(T::Tup(1, "abc"));
    ref.insert(T::Tup(2, "def"));

    for (T::Iterator it = t1.begin(); it != t1.end(); ++it) {
        s.insert(*it);
    }
    ASSERT_TRUE(ref == s);
}

TEST_F(test_basic_suite, bug1)
{
    typedef ST_TABLE(UNIT_ID, SITE_ID, ST_UNIQUE_KEY(UNIT_ID, ST_CLUSTER_KEY(SITE_ID))) Table2;
    Table2 t;
    t.init();
    t.insert(302, 1975);
    t.insert(302, 6626);
    t.insert(302, 301000);
    t.insert(302, 301001);
    t.insert(305, 1975);
    t.insert(305, 301000);
    t.insert(305, 301001);
    t.insert(306, 301000);
    t.insert(306, 301001);

    ASSERT_TRUE((t.seek<UNIT_ID, SITE_ID>(302, 1975)));
}

TEST_F(test_basic_suite, bug_index_conflict)
{
    typedef ST_TABLE(UNIT_ID, PLAN_ID, USER_ID,
                     ST_UNIQUE_KEY(UNIT_ID, ST_CLUSTER_KEY(USER_ID)),
                     ST_UNIQUE_KEY(UNIT_ID),
                     ST_UNIQUE_KEY(UNIT_ID, ST_CLUSTER_KEY(PLAN_ID))) Table2;
    bool dummy1 = CAP(c_same, Table2::ModifySeqL, ST_MAKE_LIST(Int<1>, Int<0>, Int<2>));
    ASSERT_TRUE(dummy1);

    cout << c_show(Table2::UniqueInfoM) << endl
         << c_show(Table2::CommonUniqueness) << endl;
}


template <class _Tup>
class TrivialObserver : public BaseObserver<const _Tup&> {
public:
    TrivialObserver() : cnt(0) {}
    
    void on_event (const _Tup& tup)
    {
        tup_ = tup;
        ++ cnt;
    }

    _Tup tup_;
    int cnt;
};
    
TEST_F(test_basic_suite, observers)
{
    Table1 tb1;
    tb1.init(1000, 70);
        
    TrivialObserver<Table1::Tup> insert_ob;
    tb1.insert_event.subscribe(&insert_ob);
    tb1.post_replace_event.subscribe(&insert_ob);

    TrivialObserver<Table1::Tup> erase_ob;
    tb1.erase_event.subscribe(&erase_ob);

    TrivialObserver<Table1::Tup> replace_ob;
    tb1.pre_replace_event.subscribe(&replace_ob);

    tb1.insert (1, 2, 3, 100);
    cout << show(tb1).c_str() << endl;
    ASSERT_EQ (insert_ob.cnt, 1);
    ASSERT_EQ (erase_ob.cnt, 0);
    ASSERT_EQ (replace_ob.cnt, 0);
    ASSERT_TRUE (insert_ob.tup_ == Table1::Tup(1,2,3,100));
    
    tb1.insert (1, 20, 30, 200);
    cout << show(tb1).c_str() << endl;
    ASSERT_EQ (insert_ob.cnt, 2);
    ASSERT_EQ (erase_ob.cnt, 0);
    ASSERT_EQ (replace_ob.cnt, 1);
    ASSERT_TRUE (insert_ob.tup_ == Table1::Tup(1,20,30,200));

    //tb1.erase_tuple (Table1::Tup(2, 2, 3, 100));
    tb1.erase<UNIT_ID>(2);
    ASSERT_EQ (insert_ob.cnt, 2);
    ASSERT_EQ (erase_ob.cnt, 0);
    ASSERT_EQ (replace_ob.cnt, 1);

    //tb1.erase_tuple (Table1::Tup(1, 20, 30, 200));
    tb1.erase<UNIT_ID>(1);
    ASSERT_EQ (insert_ob.cnt, 2);
    ASSERT_EQ (erase_ob.cnt, 1);
    ASSERT_EQ (replace_ob.cnt, 1);
    ASSERT_TRUE (erase_ob.tup_ == Table1::Tup(1,20,30,200));    
    
    cout << "mem: " << tb1.mem() << endl
         << "IndexL: " << c_show(Table1::IndexL) << endl
         << "IndexInfoL: " << c_show(Table1::IndexInfoL) << endl
         << "ModifySeqL: " << c_show(Table1::ModifySeqL) << endl
         << "not_init: " << tb1.not_init() << endl;
}

TEST_F(test_basic_suite, sanity)
{
    Table1 tb1;
    const Table1::Tup* p_tup = NULL;
    Table1::SeekIterator<PLAN_ID>::R it1;
    Table1::SeekIterator<USER_ID>::R it2;

    ASSERT_TRUE(tb1.not_init());
    tb1.init(1000, 70);
    ASSERT_FALSE(tb1.not_init());

    ASSERT_TRUE (Table1::N_INDEX == 3);
    ASSERT_EQ (0ul, tb1.size());
    ASSERT_TRUE(tb1.empty());
    ASSERT_EQ (0ul, tb1.key_num<UNIT_ID>());
    ASSERT_EQ (0ul, tb1.key_num<PLAN_ID>());
    ASSERT_EQ (0ul, tb1.key_num<USER_ID>());

    ASSERT_TRUE(tb1.insert(1,1,1,1));
    ASSERT_EQ(tb1.size(), 1ul);
    ASSERT_FALSE(tb1.empty());
    ASSERT_EQ (1ul, tb1.key_num<UNIT_ID>());
    ASSERT_EQ (1ul, tb1.key_num<PLAN_ID>());
    ASSERT_EQ (1ul, tb1.key_num<USER_ID>());
    
    ASSERT_TRUE(tb1.insert(2,2,1,2));
    ASSERT_EQ(tb1.size(), 2ul);
    ASSERT_FALSE(tb1.empty());
    ASSERT_EQ (2ul, tb1.key_num<UNIT_ID>());
    ASSERT_EQ (2ul, tb1.key_num<PLAN_ID>());
    ASSERT_EQ (1ul, tb1.key_num<USER_ID>());
    
    p_tup = tb1.seek<UNIT_ID>(1);
    it1 = tb1.seek<PLAN_ID>(1);
    it2 = tb1.seek<USER_ID>(1);
    ASSERT_TRUE(p_tup && *p_tup == Table1::Tup(1,1,1,1));
    ASSERT_TRUE(it1 && Table1::Tup(1,1,1,1) == *it1);
    ASSERT_FALSE(++it1);
    ASSERT_TRUE(it2 && Table1::Tup(2,2,1,2) == *it2);
    ASSERT_TRUE(++it2 && Table1::Tup(1,1,1,1) == *it2);
    ASSERT_FALSE(++it2);

    p_tup = tb1.seek<UNIT_ID>(2);
    it1 = tb1.seek<PLAN_ID>(2);
    ASSERT_TRUE(p_tup && *p_tup == Table1::Tup(2,2,1,2));
    ASSERT_TRUE(it1 && Table1::Tup(2,2,1,2) == *it1);
    ASSERT_FALSE(++it1);
    
    ASSERT_FALSE(tb1.erase<USER_ID>(2));
    ASSERT_EQ (tb1.size(), 2ul);
    ASSERT_FALSE(tb1.empty());

    ASSERT_TRUE(tb1.erase<PLAN_ID>(2));
    ASSERT_EQ (tb1.size(), 1ul);
    ASSERT_FALSE(tb1.empty());
    ASSERT_EQ (1ul, tb1.key_num<UNIT_ID>());
    ASSERT_EQ (1ul, tb1.key_num<PLAN_ID>());
    ASSERT_EQ (1ul, tb1.key_num<USER_ID>());
    
    ASSERT_TRUE(tb1.erase<USER_ID>(1));
    ASSERT_EQ (tb1.size(), 0ul);
    ASSERT_TRUE(tb1.empty());
}

TEST_F(test_basic_suite, randomly_insert_erase_clear_while_copy)
{
    Table1 tb1, tb2;
    tb1.init(1000, 70);
    
    const int MAX_UNIT_ID = 1000000;
    const int MAX_PLAN_ID = MAX_UNIT_ID / 1000;
    const int MAX_USER_ID = MAX_UNIT_ID / 100;
    const int REP_TIMES = 50000;
    
    typedef __gnu_cxx::hash_map<int32_t, Table1::Tup> UnitRef;
    UnitRef unit_ref1(MAX_UNIT_ID), unit_ref2(MAX_UNIT_ID);
    
    for (int j=0; j<40; ++j) {
        cout << "round: " << j << endl;
        {
            TIME_SCOPE_IN_US("copy_tb1_to_tb2: ");
            tb2 = tb1;
        }

        {
            TIME_SCOPE_IN_US("copy_unit_ref1_to_unit_ref2: ");
            unit_ref2 = unit_ref1;
        }
        
        int inserted_times = 0;
        int cleared_times = 0;
        int erased_times = 0;
        int eff_erased_times = 0;
        
        for (int i=0; i<REP_TIMES; ++i) {
            Table1::Tup t(rand() % MAX_UNIT_ID, rand() % MAX_PLAN_ID,
                      rand() % MAX_USER_ID, rand());
            int r = rand() % 200000;
            if (r < 120000) {
                ++ inserted_times;
                tb1.insert_tuple (t);
                unit_ref1[t.at<UNIT_ID>()] = t;
            }
            else if (r < 199999) {
                ++ erased_times;
                eff_erased_times += tb1.erase<UNIT_ID>(t.at<UNIT_ID>());
                unit_ref1.erase (t.at<UNIT_ID>());
            }
            else {  // == 19999, 1/20000 percentage
                unit_ref1.clear();
                tb1.clear();
                ++ cleared_times;
            }
        }

        cout << "inserted: " << inserted_times << endl
             << "erased: " << erased_times << endl
             << "eff_erased: " << eff_erased_times << endl
             << "cleared: " << cleared_times << endl
             << "tb1: " << show(tb1).c_str() << endl
             << "tb2: " << show(tb2).c_str() << endl
            ;
    }

    
    Table1* tbs[] = { &tb1, &tb2 };
    UnitRef* unit_refs[] = { &unit_ref1, &unit_ref2 };
    for (int i=0; i<2; ++i) {
        cout << "tb[" << i << "].size=" << tbs[i]->size()
             << ", unit_ref[" << i << "].size=" << unit_refs[i]->size()
             << endl;

        // Check from UNIT_ID
        {
            TIME_SCOPE_IN_MS("check_from_unit_id: ");

            for (Table1::Iterator it=tbs[i]->begin(), it_e=tbs[i]->end(); 
                 it != it_e; ++it) {
                UnitRef::iterator it2 = unit_refs[i]->find(it->at<UNIT_ID>());
                ASSERT_TRUE (it2 != unit_refs[i]->end());
                ASSERT_EQ (it2->second, *it);
            }

            for (UnitRef::iterator it = unit_refs[i]->begin();
                 it != unit_refs[i]->end(); ++it) {
                CAP(Table1::SeekIterator, UNIT_ID) it2 =
                    tbs[i]->seek<UNIT_ID>(it->first);
                ASSERT_TRUE (it2);
                ASSERT_EQ (it->second, *it2);
            }
            ASSERT_EQ (tbs[i]->size(), unit_refs[i]->size());
        }

        // Check from PLAN_ID
        {
            TIME_SCOPE_IN_MS("check_from_plan_id: ");

            typedef CAP(Table1::IndexAt, 2) PlanIndex2;
            C_ASSERT (CAP(is_unique_cluster_index, PlanIndex2), bad_index);
            typedef PlanIndex2::Base PlanIndex;
            const PlanIndex& plan_index = *(tbs[i]->index<2>().base());

            const PlanIndex::HMap* p_plan_map = plan_index.hash_map_ptr();
            for (PlanIndex::HMap::Iterator it=p_plan_map->begin(),
                     it_e = p_plan_map->end(); it != it_e; ++it) {
                // hkey should be a trivial tuple with PLAN_ID
                C_ASSERT (sizeof(it->hkey_) == sizeof(PLAN_ID::Type), bad);

                //Table1::SeekIterator<PLAN_ID> 
                CAP(Table1::SeekIterator, PLAN_ID)
                    cit = tbs[i]->seek<PLAN_ID>(it->hkey_.at<PLAN_ID>());

                // all clusters must not be empty
                // (empty clusters should be removed from the map)
                ASSERT_TRUE(cit);

                CAP(Table1::SeekIterator, PLAN_ID) p_last_tup;
                for ( ; cit; ++cit) {
                    // tuples should be strictly greater
                    if (p_last_tup) {
                        ASSERT_GT (*cit, *p_last_tup);
                    }
                    p_last_tup = cit;

                    // all tuples should exist in the reference
                    UnitRef::iterator it2 = unit_refs[i]->find(cit->at<UNIT_ID>());
                    ASSERT_TRUE (it2 != unit_refs[i]->end());
                    ASSERT_EQ (it2->second.at<PLAN_ID>(), it->hkey_.at<PLAN_ID>());
                    ASSERT_EQ (it2->second.at<USER_ID>(), cit->at<USER_ID>());
                    ASSERT_EQ (it2->second.at<STATUS>(), cit->at<STATUS>());

                }            
            }
            ASSERT_EQ (plan_index.size(), unit_refs[i]->size());
        }

        // Check from USER_ID, very similar with PLAN_ID except
        //   - index no. are different
        //   - clusters in reversed order
        {
            TIME_SCOPE_IN_MS("check_from_user_id: ");

            typedef CAP(Table1::IndexAt, 0) UserIndex2;
            C_ASSERT (CAP(is_unique_cluster_index, UserIndex2), bad_index);
            typedef UserIndex2::Base UserIndex;
            const UserIndex& user_index = *(tbs[i]->index<0>().base());

            const UserIndex::HMap* p_user_map = user_index.hash_map_ptr();
            for (UserIndex::HMap::Iterator it=p_user_map->begin(),
                     it_e = p_user_map->end(); it != it_e; ++it) {
                // hkey should be a trivial tuple with USER_ID
                C_ASSERT (sizeof(it->hkey_) == sizeof(USER_ID::Type), bad);

                //Table1::SeekIterator<USER_ID> 
                CAP(Table1::SeekIterator, USER_ID)
                    cit = tbs[i]->seek<USER_ID>(it->hkey_.at<USER_ID>());

                // all clusters must not be empty
                // (empty clusters should be removed from the map)
                ASSERT_TRUE(cit);

                CAP(Table1::SeekIterator, USER_ID) p_last_tup;
                for ( ; cit; ++cit) {
                    // tuples should be strictly less
                    if (p_last_tup) {
                        ASSERT_LT (*cit, *p_last_tup);
                    }
                    p_last_tup = cit;

                    // all tuples should exist in the reference
                    UnitRef::iterator it2 = unit_refs[i]->find(cit->at<UNIT_ID>());
                    ASSERT_TRUE (it2 != unit_refs[i]->end());
                    ASSERT_EQ (it2->second.at<PLAN_ID>(), cit->at<PLAN_ID>());
                    ASSERT_EQ (it2->second.at<USER_ID>(), it->hkey_.at<USER_ID>());
                    ASSERT_EQ (it2->second.at<STATUS>(), cit->at<STATUS>());
                }            
            }
            ASSERT_EQ (user_index.size(), unit_refs[i]->size());
        }
        
    }
        
}

