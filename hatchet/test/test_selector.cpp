// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com, jiangrujie@baidu.com
// Date: 2011-01-14 06:17

#include <gtest/gtest.h>
#include <set>
#include <map>
#include <ext/hash_map>
#include "c_connector.hpp"

using namespace st;

int main(int argc, char **argv)
{
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

DEFINE_COLUMN (UNIT_ID, u_int);
DEFINE_COLUMN (SITE_ID, u_int);
DEFINE_COLUMN (USER_ID, u_int);
DEFINE_COLUMN (PLAN_ID, u_int);
DEFINE_COLUMN (TRADE_ID, u_int);
DEFINE_COLUMN (STATUS, u_int);
DEFINE_COLUMN (CYCHOLD, u_int);
DEFINE_COLUMN (SIGN1, u_int);
DEFINE_COLUMN (SIGN2, u_int);
DEFINE_COLUMN (IDEA, u_int);
DEFINE_COLUMN (REGION, u_int);
DEFINE_COLUMN (KEYWORD, u_int);
DEFINE_COLUMN (TPC, u_int);
DEFINE_COLUMN (TERM, int);
DEFINE_COLUMN (ADQ, u_int);
DEFINE_COLUMN (BID, u_int);

typedef ST_TABLE(UNIT_ID, BID, ST_UNIQUE_KEY(UNIT_ID)) Table1;
typedef ST_TABLE(UNIT_ID, SITE_ID,
                 ST_UNIQUE_KEY(UNIT_ID, ST_CLUSTER_KEY(SITE_ID))) Table2;
typedef ST_TABLE(UNIT_ID, SITE_ID, BID,
                 ST_UNIQUE_KEY(SITE_ID, ST_CLUSTER_KEY(UNIT_ID))) Table3;


typedef ST_TABLE(UNIT_ID, SITE_ID, ST_UNIQUE_KEY(UNIT_ID)) SRC1;
typedef ST_TABLE(UNIT_ID, USER_ID, ADQ, ST_UNIQUE_KEY(UNIT_ID, ST_CLUSTER_KEY(USER_ID))) SRC2;
typedef ST_TABLE(UNIT_ID, USER_ID, ST_UNIQUE_KEY(UNIT_ID, ST_CLUSTER_KEY(USER_ID))) DST;

typedef ST_CONNECTOR(
        DST,
        ST_PICK(TBL2<UNIT_ID>, TBL2<USER_ID>),
        ST_FROM(SRC1, SRC2),
        ST_WHERE(
            eq(TBL1<UNIT_ID>, TBL3<UNIT_ID>),
            eq(TBL2<ADQ>, VAR<u_int>))
        ) C1;
        
            
TEST_F(test_usage_suite, bug_unfunctional_filter)
{
    SRC1 a;
    SRC2 c; 
    DST d;
    
    a.init();
    c.init();
    d.init();

    a.insert(1, 10);
    c.insert(1, 2, 10);
    c.insert(1, 3, 10);
    
    C1 con(&d, &a, &c);
    con.connect(9);

    CAP(DST::SeekIterator, UNIT_ID, USER_ID) p;
    
    ASSERT_EQ(d.size(), 0ul);
    c.insert(1, 2, 11);
    ASSERT_EQ(c.size(), 2ul);
    ASSERT_EQ(d.size(), 0ul);
}

struct pick_site_unit {
    template <class _Recv, class _Src>
    void operator() (const _Recv& recv, const _Src& src) const
    {
        recv(Table3::Tup(
                 src.template at<TBL2, UNIT_ID>(),
                 src.template at<TBL2, SITE_ID>(),
                 src.template at<TBL1, BID>()));
    }
};

TEST_F(test_usage_suite, maybe_non_hash)
{
    Table1 t1;
    t1.init();
    ASSERT_TRUE(t1.empty());

    Table2 t2;
    t2.init();
    ASSERT_TRUE(t2.empty());

    Table3 t3;
    t3.init();

    typedef ST_CONNECTOR(
        Table3,
        pick_site_unit,
        ST_FROM(Table1, Maybe<Table2>),
        ST_WHERE(eq(TBL1<UNIT_ID>, TBL2<UNIT_ID>))) Con;
    cout << "Con: " << c_show(Con) << endl;

    Con c(&t3, &t1, &t2);
    c.connect();

    CAP(Table3::SeekIterator, UNIT_ID, SITE_ID) p;

    ASSERT_TRUE(t3.empty());

    t1.insert(1, 1000);
    t1.insert(3, 3000);
    t1.insert(4, 4000);

    t2.insert(1, 10);
    t2.insert(1, 11);
    t2.insert(3, 31);
    t2.insert(3, 32);
    t2.insert(3, 33);

    for (Table3::Iterator it=t3.begin(); it; ++it) {
        cout << *it << endl;
    }
    
    ASSERT_EQ(t3.size(), 6ul);
    p = t3.seek<UNIT_ID, SITE_ID>(4, 0);
    ASSERT_TRUE(p && p->at<BID>() == 4000);
    p = t3.seek<UNIT_ID, SITE_ID>(1, 10);
    ASSERT_TRUE(p && p->at<BID>() == 1000);
    p = t3.seek<UNIT_ID, SITE_ID>(1, 11);
    ASSERT_TRUE(p && p->at<BID>() == 1000);
    p = t3.seek<UNIT_ID, SITE_ID>(3, 31);
    ASSERT_TRUE(p && p->at<BID>() == 3000);
    p = t3.seek<UNIT_ID, SITE_ID>(3, 32);
    ASSERT_TRUE(p && p->at<BID>() == 3000);
    p = t3.seek<UNIT_ID, SITE_ID>(3, 33);
    ASSERT_TRUE(p && p->at<BID>() == 3000);
    
    t2.insert(4, 40);
    ASSERT_EQ(t3.size(), 6ul);
    p = t3.seek<UNIT_ID, SITE_ID>(4, 40);
    ASSERT_TRUE(p && p->at<BID>() == 4000);
    p = t3.seek<UNIT_ID, SITE_ID>(1, 10);
    ASSERT_TRUE(p && p->at<BID>() == 1000);
    p = t3.seek<UNIT_ID, SITE_ID>(1, 11);
    ASSERT_TRUE(p && p->at<BID>() == 1000);
    p = t3.seek<UNIT_ID, SITE_ID>(3, 31);
    ASSERT_TRUE(p && p->at<BID>() == 3000);
    p = t3.seek<UNIT_ID, SITE_ID>(3, 32);
    ASSERT_TRUE(p && p->at<BID>() == 3000);
    p = t3.seek<UNIT_ID, SITE_ID>(3, 33);
    ASSERT_TRUE(p && p->at<BID>() == 3000);

    t2.erase<UNIT_ID>(3);
    ASSERT_EQ(t3.size(), 4ul);
    p = t3.seek<UNIT_ID, SITE_ID>(4, 40);
    ASSERT_TRUE(p && p->at<BID>() == 4000);
    p = t3.seek<UNIT_ID, SITE_ID>(1, 10);
    ASSERT_TRUE(p && p->at<BID>() == 1000);
    p = t3.seek<UNIT_ID, SITE_ID>(1, 11);
    ASSERT_TRUE(p && p->at<BID>() == 1000);
    p = t3.seek<UNIT_ID, SITE_ID>(3, 0);
    ASSERT_TRUE(p && p->at<BID>() == 3000);

    t2.erase<UNIT_ID, SITE_ID>(4, 40);
    ASSERT_EQ(t3.size(), 4ul);
    p = t3.seek<UNIT_ID, SITE_ID>(4, 0);
    ASSERT_TRUE(p && p->at<BID>() == 4000);
    p = t3.seek<UNIT_ID, SITE_ID>(1, 10);
    ASSERT_TRUE(p && p->at<BID>() == 1000);
    p = t3.seek<UNIT_ID, SITE_ID>(1, 11);
    ASSERT_TRUE(p && p->at<BID>() == 1000);
    p = t3.seek<UNIT_ID, SITE_ID>(3, 0);
    ASSERT_TRUE(p && p->at<BID>() == 3000);
    
}


struct Point {
    u_int x;
    u_int y;
    Point(u_int x1, u_int y1) : x(x1), y(y1) {}
    bool operator== (Point const& pt) const
    { return x == pt.x && y == pt.y; }
    bool operator< (Point const& rhs) const
    { return x != rhs.x ? x < rhs.x : y < rhs.y; }
};

namespace __gnu_cxx {
template <> struct hash<Point> {
    size_t operator() (Point const& pt) const {
        return pt.x * 257 + pt.y;
    }
};
}

TEST_F(test_usage_suite, maybe_selection)
{
    typedef ST_TABLE(
        UNIT_ID, SITE_ID, STATUS,
        ST_UNIQUE_KEY(SITE_ID, ST_CLUSTER_KEY(UNIT_ID)),
        ST_UNIQUE_KEY(UNIT_ID)) UnitTable;
    
    UnitTable t1, t2, t3;
    t1.init();
    t2.init();
    t3.init();
    
    for (int i=1; i<10; ++i) {
        t1.insert(i, i, i);
        t3.insert(i, i, i);
    }
    t2.insert(1, 1, 10);
    t2.insert(2, 1, 20);
    t2.insert(4, 4, 40);
    
    typedef ST_SELECTOR(
        ST_FROM(UnitTable, Maybe<UnitTable, 1>, UnitTable),
        ST_WHERE(eq(TBL1<UNIT_ID>, TBL2<UNIT_ID>),
                 eq(TBL1<UNIT_ID>, TBL3<UNIT_ID>))) Sel;
    cout << "Sel: " << c_show(Sel) << endl;
    
    Sel s(&t1, &t2, &t3);

    for (Sel::Iterator it = s.select(); it; ++it) {
        bool r = it.at<TBL1,UNIT_ID>() == it.at<TBL2,UNIT_ID>() &&
            it.at<TBL1,UNIT_ID>() == it.at<TBL3,UNIT_ID>();
        ASSERT_TRUE(r);
        cout << it.at<TBL1, UNIT_ID>()
             << ", " << it.at<TBL1, STATUS>()
             << ", " << it.at<TBL2, STATUS>()
             << ", " << it.at<TBL3, STATUS>()
             << endl;
    }
    cout << endl;

    t1.insert(5, 2, 50);
    for (Sel::Iterator it = s.select(); it; ++it) {
        cout << it.at<TBL1, UNIT_ID>()
             << ", " << it.at<TBL1, STATUS>()
             << ", " << it.at<TBL2, STATUS>()
             << ", " << it.at<TBL3, STATUS>()
             << endl;
    }
    cout << endl;

    t2.insert(5, 3, 50);
    for (Sel::Iterator it = s.select(); it; ++it) {
        cout << it.at<TBL1, UNIT_ID>()
             << ", " << it.at<TBL1, STATUS>()
             << ", " << it.at<TBL2, STATUS>()
             << ", " << it.at<TBL3, STATUS>()
             << endl;
    }
    cout << endl;
}

TEST_F(test_usage_suite, maybe_connection)
{
    typedef ST_TABLE(UNIT_ID, STATUS, ST_UNIQUE_KEY(UNIT_ID)) UnitTable;
    typedef ST_TABLE(UNIT_ID, SIGN1, SIGN2,
                     ST_UNIQUE_KEY(SIGN1, SIGN2)) SignTable;
    typedef __gnu_cxx::hash_map<Point, u_int> Ref;

    SignTable dt;
    UnitTable t1, t2;
    Ref ref;
    
    t1.init();
    t2.init();
    dt.init();

    for (int i=0; i<10; ++i) {
        t1.insert(i, i);
        t2.insert(i, i);
        ref[Point(i,i)] = i;
    }

    typedef ST_CONNECTOR(
        SignTable,
        ST_PICK(TBL1<UNIT_ID>, TBL1<STATUS>, TBL2<STATUS>),
        ST_FROM (UnitTable, Maybe<UnitTable>),
        ST_WHERE (eq(TBL1<UNIT_ID>, TBL2<UNIT_ID>))) Con;
    cout << "Con: " << c_show(Con) << endl;

    Con s(&dt, &t1, &t2);
    s.connect();

    for (int i=0; i<10; ++i) {
        switch (i) {
        case 0:
            cout << "insert_t2: present in t2" << endl;
            t2.insert(1, 10);
            ref.erase(Point(1,1));
            ref[Point(1,10)] = 1;
            
            t2.insert(2, 20);
            ref.erase(Point(2,2));
            ref[Point(2,20)] = 2;
            break;
        case 1:
            cout << "insert_t2: absent in t2" << endl;
            t2.insert(1000, 1000);
            t2.insert(1001, 1001);
            // ref is unchanged
            break;
        case 2:
            cout << "insert_t1: absent in t1/t2" << endl;
            t1.insert(100, 100);
            ref[Point(100, ST_DEFAULT_VALUE_OF(u_int))] = 100;
                      
            t1.insert(101, 101);
            ref[Point(101, ST_DEFAULT_VALUE_OF(u_int))] = 101;
            break;
        case 3:
            cout << "insert_t1: absent in t1, present in t2" << endl;
            t1.insert(1001, 1001);
            ref.erase(Point(1001, ST_DEFAULT_VALUE_OF(u_int)));
            ref[Point(1001, 1001)] = 1001;
            break;
        case 4:
            cout << "erase_t2: present in t1/t2" << endl;
            t2.erase<UNIT_ID>(1001);
            ref.erase(Point(1001, 1001));
            ref[Point(1001, ST_DEFAULT_VALUE_OF(u_int))] = 1001;
            break;
        }
        
        for (SignTable::Iterator it=dt.begin(), it_e=dt.end(); 
             it != it_e; ++it) {
            Ref::iterator it2 =
                ref.find(Point(it->at<SIGN1>(), it->at<SIGN2>()));
            ASSERT_TRUE (it2 != ref.end());
            ASSERT_EQ (it2->second, it->at<UNIT_ID>());
        }

        for (Ref::iterator it = ref.begin();
             it != ref.end(); ++it) {
            CAP(SignTable::SeekIterator, SIGN1, SIGN2) it2 =
                dt.seek<SIGN1, SIGN2>(it->first.x, it->first.y);
            ASSERT_TRUE (it2);
            ASSERT_EQ (it->second, it2->at<UNIT_ID>());
        }
        ASSERT_EQ (dt.size(), ref.size());
    }
    

    // for (SignTable::iterator it=dt.begin(); it!=dt.end(); ++it) {
    //     cout << it->at<UNIT_ID>()
    //          << ", " << it->at<SIGN1>()
    //          << ", " << it->at<SIGN2>()
    //          << endl;
    // }
    // cout << endl;

}


typedef ST_TABLE(UNIT_ID, TERM,
                 ST_UNIQUE_KEY(UNIT_ID, ST_CLUSTER_KEY(TERM))) UnitTermTable;

typedef ST_TABLE(PLAN_ID, STATUS,
                 ST_UNIQUE_KEY(PLAN_ID)) PlanStatTable;

typedef ST_TABLE(USER_ID, STATUS,
                 ST_UNIQUE_KEY(USER_ID)) UserStatTable;
    
typedef ST_TABLE(UNIT_ID, STATUS, PLAN_ID, USER_ID,
                 ST_UNIQUE_KEY(PLAN_ID, ST_CLUSTER_KEY(UNIT_ID)),
                 ST_UNIQUE_KEY(USER_ID, ST_CLUSTER_KEY(UNIT_ID)),
                 ST_UNIQUE_KEY(UNIT_ID)) UnitStatTable;

typedef ST_TABLE(STATUS, ST_UNIQUE_KEY(STATUS)) TriggeredTable;

struct check_status {
    bool operator() (u_int st) const
    { return (st & 0x1) == 0; }
};

struct user_pick {
    user_pick() {}
    
    template <class _Recv, class _Source>
    void operator() (const _Recv& r, const _Source& src)
    {
        int sign = src.template at<TBL1, UNIT_ID>() << 24
            | src.template at<TBL1, TERM>() << 16
            | src.template at<TBL2, PLAN_ID>() << 8
            | src.template at<TBL3, USER_ID>();
        r(TriggeredTable::Tup(sign));
    }
};

TEST_F(test_usage_suite, mutiple_tables)
{
    srand(time(0));
    
    // A set used to store the results.
    typedef std::set<u_int> ref_t;
    
    UnitTermTable table1;
    table1.init (1000, 80);
    for(int i = 0; i < 200; i++) {
        table1.insert(rand() % 200, rand() % 100);
    }

    PlanStatTable table2;
    table2.init (1000, 80);
    for(int i = 0; i < 80; i++) {
        table2.insert(rand() % 100, rand() % 100);
    }

    UserStatTable table3;
    table3.init (1000, 80);
    for(int i = 0; i < 80; i++) {
        table3.insert(rand() % 100, rand() % 100);
    }
    
    UnitStatTable table4;
    table4.init (1000, 80);
    for(int i = 0; i < 200; i++) {
        table4.insert(rand() % 200, rand() % 100, rand() % 100, rand() % 100);
    }

    {
        cout << endl << "originate: no variable" << endl;        

        typedef ST_SELECTOR(
            ST_FROM(UnitTermTable, PlanStatTable, UserStatTable, UnitStatTable),
            ST_WHERE(eq(TBL1<UNIT_ID>, TBL4<UNIT_ID>),
                     eq(TBL4<PLAN_ID>, TBL2<PLAN_ID>),
                     eq(TBL4<USER_ID>, TBL3<USER_ID>),
                     check_status(TBL2<STATUS>))) Sel1;
        cout << "DomG: " << c_show(Sel1::DomG) << endl;
        cout << "FPredL: " << c_show(Sel1::FPredL) << endl;
        cout << "VPredL: " << c_show(Sel1::VPredL) << endl;
        cout << "TSL: " << c_show(Sel1::TSL) << endl;
        cout << "VarTup: " <<c_show(Sel1::VarTup) << endl;

        Sel1 s(&table1, &table2, &table3, &table4);
        check_status cs;
        ref_t results;

        // Select manually.
        for(UnitTermTable::Iterator it1 = table1.begin(); 
            it1; ++it1) {
            for(PlanStatTable::Iterator it2 = table2.begin(); 
                it2; ++it2) {
                for(UserStatTable::Iterator it3 = table3.begin(); 
                    it3; ++it3) {
                    for(UnitStatTable::Iterator it4 = table4.begin(); 
                        it4; ++it4) {
                        if(it1->at<UNIT_ID>() == it4->at<UNIT_ID>() 
                           && it4->at<PLAN_ID>() == it2->at<PLAN_ID>()
                           && it4->at<USER_ID>() == it3->at<USER_ID>()
                           && cs(it2->at<STATUS>())) {
                            // Unique signature.
                            u_int sign = it1->at<UNIT_ID>() << 24
                                | it1->at<TERM>() << 16
                                | it2->at<PLAN_ID>() << 8
                                | it3->at<USER_ID>();
                            results.insert(sign);
                        }
                    }
                }
            }
        }
                       
        size_t size = 0;
        for (Sel1::Iterator it = s.select (); it; ++it) {
            u_int sign = it.at<TBL1, UNIT_ID>() << 24
                | it.at<TBL1, TERM>() << 16
                | it.at<TBL2, PLAN_ID>() << 8
                | it.at<TBL3, USER_ID>();
            ASSERT_TRUE(results.find(sign) != results.end());
            size++;
        }
        ASSERT_EQ(size, results.size());
        cout << size << "tuples returned" << endl;
    }
    
    {
        cout << endl << "originate: from plan_id" << endl;        

        typedef ST_SELECTOR(
            ST_FROM(UnitTermTable, PlanStatTable, UnitStatTable),
            ST_WHERE(eq(TBL1<UNIT_ID>, TBL3<UNIT_ID>),
                     eq(TBL3<PLAN_ID>, TBL2<PLAN_ID>),
                     eq(TBL2<PLAN_ID>, VAR<int>))) Sel1;
        cout << "DomG: " << c_show(Sel1::DomG) << endl;
        cout << "FPredL: " << c_show(Sel1::FPredL) << endl;
        cout << "VPredL: " << c_show(Sel1::VPredL) << endl;
        cout << "TSL: " << c_show(Sel1::TSL) << endl;
        cout << "VarTup: " <<c_show(Sel1::VarTup) << endl;

        Sel1 s(&table1, &table2, &table4);
        ref_t results;

        for(int i = 0 ; i < 10; i++) {
            u_int var = rand() % 100;
            
            // Select manually.
            for(UnitTermTable::Iterator it1 = table1.begin(); 
                it1; ++it1) {
                for(PlanStatTable::Iterator it2 = table2.begin(); 
                    it2; ++it2) {
                    for(UnitStatTable::Iterator it3 = table4.begin(); 
                        it3; ++it3) {
                        if(it1->at<UNIT_ID>() == it3->at<UNIT_ID>() 
                           && it2->at<PLAN_ID>() == var
                           && it3->at<PLAN_ID>() == it2->at<PLAN_ID>()) {
                            // Unique signature.
                            u_int sign = it1->at<UNIT_ID>() << 8
                                | it1->at<TERM>();
                            results.insert(sign);
                        }
                    }
                }
            }

            size_t size = 0;
            for (Sel1::Iterator it = s.select (var); it; ++it) {
                u_int sign = it.at<TBL1, UNIT_ID>() << 8
                    | it.at<TBL1, TERM>();
                ASSERT_TRUE(results.find(sign) != results.end());
                size++;
            }
            ASSERT_EQ(size, results.size());
            cout << size << "tuples returned" << endl;
            results.clear();
        }
    }

    { 
        cout << endl << "connector: no variable" << endl; 
        TriggeredTable dest_tbl;
        dest_tbl.init (1000, 80);

        typedef ST_CONNECTOR(
            TriggeredTable,
            user_pick,
            ST_FROM(UnitTermTable, PlanStatTable,
                    UserStatTable, UnitStatTable ),
            ST_WHERE(ge(TBL1<TERM>, VAR<int>),
                     eq(TBL1<UNIT_ID>, TBL4<UNIT_ID>),
                     eq(TBL4<PLAN_ID>, TBL2<PLAN_ID>),
                     eq(TBL4<USER_ID>, TBL3<USER_ID>),
                     check_status(TBL2<STATUS>))) Tri1;
        cout << "VarTup: " <<c_show(Tri1::VarTup) << endl;
        cout << "DomG: " << c_show(Tri1::DomG) << endl;
        cout << "FPredL: " << c_show(Tri1::FPredL) << endl;

        Tri1 s(&dest_tbl, &table1, &table2, &table3, &table4);
        check_status cs;
        ref_t results;
        
        typedef CAP(list_at, 0, Tri1::TrigSelL) TrigSel1;
        cout << "TrigSel1:" << endl;
        cout << "PreFPredL: " << c_show(TrigSel1::PreFPredL) << endl;
        cout << "TSL: " << c_show(TrigSel1::TSL) << endl;

        cout << c_show(Tri1) << endl;

        s.connect(10);
        //int a, b, c, d, e;
        for(int i = 0 ; i < 5; i++) {
            // Select manually.
            for(UnitTermTable::Iterator it1 = table1.begin(); 
                it1; ++it1) {
                for(PlanStatTable::Iterator it2 = table2.begin(); 
                    it2; ++it2) {
                    for(UserStatTable::Iterator it3 = table3.begin(); 
                        it3; ++it3) {
                        for(UnitStatTable::Iterator it4 = table4.begin(); 
                            it4; ++it4) {
                            if(it1->at<UNIT_ID>() == it4->at<UNIT_ID>() 
                               && it4->at<PLAN_ID>() == it2->at<PLAN_ID>()
                               && it4->at<USER_ID>() == it3->at<USER_ID>()
                               && cs(it2->at<STATUS>())
                               && it1->at<TERM>() >= 10) {
                                // Unique signature.
                                u_int sign = it1->at<UNIT_ID>() << 24
                                    | it1->at<TERM>() << 16
                                    | it2->at<PLAN_ID>() << 8
                                    | it3->at<USER_ID>();
                                results.insert(sign);
                            }
                        }
                    }
                }
            }

            size_t size = 0;
            for (TriggeredTable::Iterator it = dest_tbl.begin(); it; ++it) {
                u_int sign = it->at<STATUS>();
                ASSERT_TRUE(results.find(sign) != results.end());
                size++;
            }

            ASSERT_EQ(size, results.size());
            cout << size << "tuples in all" << endl;

            for(int j = 0; j < 10; j++) {
                if(rand() % 100 >= 20) {
                    table1.insert(rand() % 200, rand() % 100);
                    table2.insert(rand() % 100, rand() % 100);
                    table3.insert(rand() % 100, rand() % 100);
                    table4.insert(rand() % 200, rand() % 100, 
                                  rand() % 100, rand() % 100);
                    table4.insert(rand() % 200, rand() % 100, 
                                  rand() % 100, rand() % 100);
                } else {
                    table1.erase<UNIT_ID>(rand() % 200);
                    table2.erase<PLAN_ID>(rand() % 100);
                    table3.erase<USER_ID>(rand() % 100);
                    table4.erase<UNIT_ID>(rand() % 200);
                    table4.erase<UNIT_ID>(rand() % 200);
                }
            }
                
            results.clear();
        }

    }
}

// TEST_F(test_usage_suite, one_table_connection)
// {
//     typedef ST_TABLE(UNIT_ID, SITE_ID,
//                      ST_UNIQUE_KEY(UNIT_ID, ST_CLUSTER_KEY(SITE_ID))) T1;
//     typedef ST_TABLE(UNIT_ID, SITE_ID,
//                      ST_UNIQUE_KEY(SITE_ID, ST_CLUSTER_KEY(UNIT_ID))) T2;
//     typedef ST_CONNECTOR(
//         T2 ,
//         ST_PICK(TBL1<UNIT_ID>, TBL1<SITE_ID>),
//         ST_FROM(T1),
//         ST_WHERE(check_status(TBL1<SITE_ID>))) SingleCon;
//     T1 t1;
//     T2 t2;
//     SingleCon sc(&t2, &t1);
// }
