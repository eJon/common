// Copyright (c) 2010, 2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: Wed Jan 26 16:10:58 CST 2011

#include <gtest/gtest.h>
#include "named_tuple.hpp"
#include "st_timer.h"
#include "foo.h"
#include "combined_tuple.hpp"

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

DEFINE_ATTRIBUTE (PLAN_ID, int);
DEFINE_ATTRIBUTE (SITE_ID, long);
DEFINE_ATTRIBUTE (BID, char);
DEFINE_ATTRIBUTE (PRICE, short);
DEFINE_ATTRIBUTE (UNIT_ID, long long);

struct print_any {
    template <typename _Any>
    void operator() (const _Any& x) const
    { cout << ' ' << x; }
};

TEST_F(test_usage_suite, storage_layout)
{
    typedef make_tuple<PLAN_ID, SITE_ID, BID, PRICE, UNIT_ID>::R Tup;

    struct RefStruct {
        PLAN_ID::Type plan_id;
        SITE_ID::Type site_id;
        BID::Type bid;
        PRICE::Type price;
        UNIT_ID::Type unit_id;
    };

    ASSERT_EQ (sizeof(RefStruct), sizeof(Tup));

    ASSERT_TRUE (CAP(Tup::offset, PLAN_ID) == offsetof(RefStruct, plan_id));
    ASSERT_TRUE (CAP(Tup::offset, SITE_ID) == offsetof(RefStruct, site_id));
    ASSERT_TRUE (CAP(Tup::offset, UNIT_ID) == offsetof(RefStruct, unit_id));
    ASSERT_TRUE (CAP(Tup::offset, PRICE) == offsetof(RefStruct, price));
    ASSERT_TRUE (CAP(Tup::offset, BID) == offsetof(RefStruct, bid));
}

TEST_F(test_usage_suite, combined_tuple)
{
    typedef make_tuple<UNIT_ID, PLAN_ID>::R Tup1;
    typedef make_tuple<SITE_ID, PRICE>::R Tup2;
    typedef CombinedTuple<Tup1, Tup2> CTup;
    Tup1 t1(1, 2);
    Tup2 t2(3, 4);

    CTup ct1(&t1, &t2);
    C_ASSERT_SAME(CTup::AttrS, ST_MAKE_LIST(UNIT_ID, PLAN_ID, SITE_ID, PRICE),
        bad);
    ASSERT_EQ(ct1.at<SITE_ID>(), t2.at<SITE_ID>());
    ASSERT_EQ(ct1.at<PRICE>(), t2.at<PRICE>());
    ASSERT_EQ(ct1.at<UNIT_ID>(), t1.at<UNIT_ID>());
    ASSERT_EQ(ct1.at<PLAN_ID>(), t1.at<PLAN_ID>());

    CTup ct2 = ct1;
    ASSERT_EQ(ct2, ct1);

    cout << ct2 << endl;
}

TEST_F(test_usage_suite, map_and_fold)
{
    typedef make_tuple<PLAN_ID, SITE_ID, BID, PRICE, UNIT_ID>::R Tup;

    //cout << "test_base=" << __is_base_of (Tup::Base, Tup) << endl;
    Tup t(1, 2, 3, 4, 5);
    EXPECT_STREQ ("(1 2  4 5)", show(t).c_str());
    
    t.do_map (print_any());

    ASSERT_EQ (115, t.foldl (plus<int>(), 100));

    typedef CAP(make_list, int, long, short) TypeL;
    BasicTuple<TypeL> btup;
    init_tuple (&btup, 1, 2, 3);
    EXPECT_STREQ ("(1 2 3)", show(btup).c_str());
}

// TODO: Compare with native structs
/*
TEST_F(test_usage_suite, compare_with_tuple2)
{
    srand (time(0));
    typedef make_tuple<PLAN_ID, SITE_ID, BID, PRICE, UNIT_ID>::R Tup3;
    cout << Tup3::offset<PLAN_ID>::R << endl;
    cout << Tup3::offset<SITE_ID>::R << endl;
    cout << Tup3::offset<BID>::R << endl;
    cout << Tup3::attr_pos<PLAN_ID>::R << endl;
    cout << Tup3::attr_pos<SITE_ID>::R << endl;
    cout << Tup3::attr_pos<BID>::R << endl;

    Tup3 t;
    t.at<PLAN_ID>() = 1;
    cout << "set PLAN_ID=" << t << endl;
    t.at<SITE_ID>() = 2;
    cout << "set SITE_ID=" << t << endl;
    t.at<BID>() = 3;
    
    cout << "set BID=" << t << endl;

    typedef Tup3::make_sub<SITE_ID>::R sf_t;
    //sf_t::Tup3le t2;
    sf_t sf;
    cout << sf(t) << endl;
    
    const int LEN = 1000000;

    std::vector<u_int> a_rand;
    u_int* p_rand = NULL;

    a_rand.reserve (LEN*5);
    for (int i=0; i<LEN*5; ++i) {
        a_rand.push_back (rand());
    };
    
    p_rand = &a_rand[0];
    Tup3* a_tup3 = new Tup3[LEN];
    for (int i=0; i<LEN; ++i, ++p_rand) {
        a_tup3[i].at<BID>() = *p_rand;
        a_tup3[i].at<PLAN_ID>() = *p_rand;
        a_tup3[i].at<SITE_ID>() = *p_rand;
        a_tup3[i].at<UNIT_ID>() = *p_rand;
        a_tup3[i].at<PRICE>() = *p_rand;
    }

    typedef tuple_t<PLAN_ID, SITE_ID, BID, PRICE, UNIT_ID> Tup2;

    ASSERT_EQ (sizeof(Tup2), sizeof(Tup3));
    
    p_rand = &a_rand[0];
    Tup2* a_tup2 = new Tup2[LEN];
    for (int i=0; i<LEN; ++i, ++p_rand) {
        a_tup2[i].at<BID>() = *p_rand;
        a_tup2[i].at<PLAN_ID>() = *p_rand;
        a_tup2[i].at<SITE_ID>() = *p_rand;
        a_tup2[i].at<UNIT_ID>() = *p_rand;
        a_tup2[i].at<PRICE>() = *p_rand;
    }
    
    NaiveTimer tm;
    int s;

    tm.start ();
    s = 0;
    for (int i=0; i<LEN; ++i) {
        s += a_tup3[i].at<BID>();
    }
    tm.stop();
    cout << "tuple3.at=" << tm.u_elapsed()*1000.0/LEN
         << " " << s << endl;

    tm.start ();
    s = 0;
    for (int i=0; i<LEN; ++i) {
        s += a_tup2[i].at<BID>();
    }
    tm.stop();
    cout << "tuple2.at=" << tm.u_elapsed()*1000.0/LEN
         << " " << s << endl;
 
    tm.start ();
    s = 0;
    for (int i=0; i<LEN-1; ++i) {
        s += (a_tup3[i] == a_tup3[i+1]);
    }
    tm.stop();
    cout << "tuple3.equal=" << tm.u_elapsed()*1000.0/(LEN-1)
         << " " << s << endl;    

    tm.start ();
    s = 0;
    for (int i=0; i<LEN-1; ++i) {
        s += (a_tup2[i] == a_tup2[i+1]);
    }
    tm.stop();
    cout << "tuple2.equal=" << tm.u_elapsed()*1000.0/(LEN-1)
         << " " << s << endl;

    tm.start ();
    s = 0;
    for (int i=0; i<LEN-1; ++i) {
        s += valcmp(a_tup3[i], a_tup3[i+1]);
    }
    tm.stop();
    cout << "tuple3.compare=" << tm.u_elapsed()*1000.0/(LEN-1)
         << " " << s << endl;  
    
    tm.start ();
    s = 0;
    for (int i=0; i<LEN-1; ++i) {
        s += valcmp(a_tup2[i], a_tup2[i+1]);
    }
    tm.stop();
    cout << "tuple2.compare=" << tm.u_elapsed()*1000.0/(LEN-1)
         << " " << s << endl;

    tm.start ();
    s = 0;
    for (int i=0; i<LEN; ++i) {
        s += hash(a_tup3[i]);
    }
    tm.stop();
    cout << "tuple3.hash=" << tm.u_elapsed()*1000.0/LEN << " " << s << endl;
    
    tm.start ();
    s = 0;
    for (int i=0; i<LEN; ++i) {
        s += hash(a_tup2[i]);
    }
    tm.stop();
    cout << "tuple2.hash=" << tm.u_elapsed()*1000.0/LEN << " " << s << endl;
    
    delete [] a_tup3;
    delete [] a_tup2;
}
*/

TEST_F(test_usage_suite, set_and_get_and_comparison)
{
    srand (time(0));

    make_tuple<UNIT_ID, SITE_ID, BID, PLAN_ID, PRICE>::R t;

    
    cout << "sizeof(tup)=" << sizeof(t) << endl;
    t.at<UNIT_ID>() = 1;
    t.at<SITE_ID>() = 2;
    t.at<BID>() = 3;
    t.at<PLAN_ID>() = 100;
    t.at<PRICE>() = 5;

    ASSERT_TRUE (t.at_n<0>() == t.at<UNIT_ID>());
    ASSERT_TRUE (t.at_n<1>() == t.at<SITE_ID>());
    ASSERT_TRUE (t.at_n<2>() == t.at<BID>());
    ASSERT_TRUE (t.at_n<3>() == t.at<PLAN_ID>());
    ASSERT_TRUE (t.at_n<4>() == t.at<PRICE>());

    cout << t << endl;

    ASSERT_EQ (t.at<UNIT_ID>(), 1);
    ASSERT_EQ (t.at<SITE_ID>(), 2);
    ASSERT_EQ (t.at<PLAN_ID>(), 100l);

    typeof(t) tup2 = t;
    ASSERT_EQ (t, tup2);

    
    tup2.at<UNIT_ID>() = 0;
    tup2.at<PLAN_ID>() = 191;
    cout << t << ',' << tup2 << endl;

    ASSERT_TRUE (t > tup2);
    ASSERT_TRUE (t >= tup2);
    ASSERT_TRUE (t != tup2);
}


DEFINE_ATTRIBUTE (WHATEVER, foo);
DEFINE_ATTRIBUTE (TEST_ATRR, char);


// Test all kinds of defination of tuple
// Date: 18/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, defination)
{
    typedef make_tuple<PLAN_ID, UNIT_ID>::R       Tup1;
    Tup1      t1;
    t1.at<PLAN_ID>() = 1;
    t1.at<UNIT_ID>() = 2;
    EXPECT_EQ(1, t1.at<PLAN_ID>());
    EXPECT_EQ(2, t1.at<UNIT_ID>());

    typedef make_tuple<PLAN_ID, WHATEVER>::R      Tup2;
    Tup2      t2;
    t2.at<PLAN_ID>() = 1;
    t2.at<WHATEVER>() = foo(1, 2);
    EXPECT_EQ(1, t2.at<PLAN_ID>());
    EXPECT_STREQ("(1,2)", show(t2.at<WHATEVER>()).c_str())<< "foo content:(1, 2)";

    typedef make_tuple<void, PLAN_ID, UNIT_ID>::R        Tup3;
    Tup3      t3;
    t3.at<PLAN_ID>() = 1;
    t3.at<UNIT_ID>() = 2;
    EXPECT_STREQ("(1 2)", show(t3).c_str())<< "content of make_tuple<void, PLAN_ID, UNIT_ID>::R";

}

struct test_Tup1
{
    long    plan_id;
    int     unit_id1;
    int     unit_id2;
    test_Tup1():plan_id(0), unit_id1(0), unit_id2(0){}
    test_Tup1(long a, int b, int c):plan_id(a), unit_id1(b), unit_id2(c){}
};

// Test all kinds of defination of tuple
// Date: 18/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, bad_defination)
{
    //cut content2(struct)
    typedef make_tuple<PLAN_ID, WHATEVER>::R	Tup3;
    struct RefStruct { PLAN_ID::Type plan_id; WHATEVER::Type ever; };
    
    Tup3	t3;
    t3.at<PLAN_ID>() = 234;
    // Overflow on purpose
    t3.at<WHATEVER>() = foo(1, 4294967296);
    ASSERT_EQ(234, t3.at<PLAN_ID>());
    ASSERT_EQ(sizeof(RefStruct), sizeof(t3));
    EXPECT_STREQ("(234 (1,0))", show(t3).c_str());
}

DEFINE_ATTRIBUTE(FOO2, foo2);


// Test set and get method
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, set_and_get)
{
    //get normal struct
    typedef make_tuple<PLAN_ID, WHATEVER>::R	Tup1;
    Tup1	t1;

    t1.at<PLAN_ID>() = 100;
    t1.at<WHATEVER>() = foo(1, 2);

    EXPECT_STREQ("(1,2)", show(t1.at<WHATEVER>()).c_str());

    //get bit struct
    typedef make_tuple<PLAN_ID, FOO2>::R	Tup2;
    Tup2	t2;
	
    t2.at<PLAN_ID>() = 1;
    t2.at<FOO2>() = foo2(1, 2, 3, 1);
	
    EXPECT_EQ(1, t2.at<FOO2>().get_A());
    EXPECT_EQ(2, t2.at<FOO2>().get_B());
    EXPECT_EQ(3, t2.at<FOO2>().get_C());
    EXPECT_EQ(1, t2.at<FOO2>().get_D());	
}


// Test sizeof and offsetof
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, size_and_offset)
{
    typedef make_tuple<PLAN_ID, FOO2>::R      Tup1;
    struct RefStruct { PLAN_ID::Type plan_id; FOO2::Type foo2; };

    Tup1      t1;

    t1.at<PLAN_ID>() = 100;
    t1.at<FOO2>() = foo2(100, 2, 100, 1);
	
    ASSERT_EQ(sizeof(RefStruct), sizeof(t1));
    EXPECT_TRUE(offsetof(RefStruct, plan_id) == Tup1::offset<PLAN_ID>::R);
    EXPECT_TRUE(offsetof(RefStruct, foo2) == Tup1::offset<FOO2>::R);
	
    EXPECT_TRUE (0 == Tup1::attr_pos<PLAN_ID>::R);
    EXPECT_TRUE (1 == Tup1::attr_pos<FOO2>::R);
}


// Test sub tuple, mainly on get set
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, sub_tuple_get_set)
{
    typedef make_tuple<PLAN_ID, UNIT_ID, WHATEVER>::R      Tup1;
    Tup1::make_sub<UNIT_ID, WHATEVER>::R	SubFunc;
    typedef make_tuple<UNIT_ID, WHATEVER>::R	Tup2;

    Tup1	t1;
    Tup2	t2;

    t1.at<PLAN_ID>() = 10;
    t1.at<UNIT_ID>() = 2;
    t1.at<WHATEVER>() = foo(1, 2);

    t2 = SubFunc(t1);

    ASSERT_EQ(2, t2.at<UNIT_ID>());
    ASSERT_STREQ("(1,2)", show(t2.at<WHATEVER>()).c_str());
}



// Test sub tuple, mainly on size and offsetof
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, sub_tuple_size_offset)
{
    typedef make_tuple<PLAN_ID, UNIT_ID, FOO2>::R	Tup1;
    typedef Tup1::make_sub<UNIT_ID, FOO2>::R Sub;
    Sub SubFunc;
    typedef make_tuple<UNIT_ID, void, FOO2>::R Tup2;
    struct RefStruct {
        UNIT_ID::Type unit_id;
        FOO2::Type foo2;
    };

    Tup1      t1;
    Tup2      t2;

    t1.at<PLAN_ID>() = 1;
    t1.at<UNIT_ID>() = 2;
    t1.at<FOO2>() = foo2(1, 2, 3, 1);

    t2 = SubFunc(t1);

    ASSERT_EQ(2, t2.at<UNIT_ID>());
    EXPECT_EQ(1, t2.at<FOO2>().get_A());
    EXPECT_EQ(2, t2.at<FOO2>().get_B());
    EXPECT_EQ(3, t2.at<FOO2>().get_C());
    EXPECT_EQ(1, t2.at<FOO2>().get_D());
    
    ASSERT_EQ(sizeof(RefStruct), sizeof(t2));
    EXPECT_TRUE(offsetof(RefStruct, unit_id) == Tup2::offset<UNIT_ID>::R);
    EXPECT_TRUE(offsetof(RefStruct, foo2) == Tup2::offset<FOO2>::R);
    EXPECT_TRUE(0 == Tup2::attr_pos<UNIT_ID>::R);
    EXPECT_TRUE(1 == Tup2::attr_pos<FOO2>::R);	
}


// Test remove void appeared ahead of param list
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, removal_void_ahead)
{
    typedef make_tuple<void, PLAN_ID, UNIT_ID>::R	Tup1;
    struct RefStruct { PLAN_ID::Type plan_id; UNIT_ID::Type unit_id; };

    Tup1	t1;

    t1.at<PLAN_ID>() = 100;
    t1.at<UNIT_ID>() = 1000;
	
    ASSERT_EQ(sizeof(RefStruct), sizeof(t1));
    ASSERT_EQ(100, t1.at<PLAN_ID>());
    ASSERT_EQ(1000, t1.at<UNIT_ID>());
    ASSERT_TRUE(0 == Tup1::attr_pos<PLAN_ID>::R);
    ASSERT_TRUE(1 == Tup1::attr_pos<UNIT_ID>::R);
    ASSERT_TRUE(offsetof(RefStruct, plan_id) == Tup1::offset<PLAN_ID>::R);
    ASSERT_TRUE(offsetof(RefStruct, unit_id) == Tup1::offset<UNIT_ID>::R);
}


// Test remove void appeared in-middle of param list
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, removal_void_middle)
{
    typedef make_tuple<PLAN_ID, void, void, UNIT_ID, void>::R       Tup1;
    struct RefStruct { PLAN_ID::Type plan_id; UNIT_ID::Type unit_id; };

    Tup1      t1;

    
    t1.at<PLAN_ID>() = 100;
    t1.at<UNIT_ID>() = 1000;

    ASSERT_EQ(sizeof(RefStruct), sizeof(t1));
    ASSERT_EQ(100, t1.at<PLAN_ID>());
    ASSERT_EQ(1000, t1.at<UNIT_ID>());
    ASSERT_TRUE(0 == Tup1::attr_pos<PLAN_ID>::R);
    ASSERT_TRUE(1 == Tup1::attr_pos<UNIT_ID>::R);
    ASSERT_TRUE(offsetof(RefStruct, plan_id) == Tup1::offset<PLAN_ID>::R);
    ASSERT_TRUE(offsetof(RefStruct, unit_id) == Tup1::offset<UNIT_ID>::R);
}


// Test to string
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, to_string)
{
    typedef make_tuple<PLAN_ID, WHATEVER>::R	Tup1;
    Tup1      t1;

    t1.at<PLAN_ID>() = 100;
    t1.at<WHATEVER>() = foo(23, 1);

    ASSERT_STREQ("(100 (23,1))", show(t1).c_str());
}


// Test comapre operator
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, test_comapre)
{
    typedef make_tuple<PLAN_ID, WHATEVER>::R       Tup1;

    //operator <
    Tup1      t1, t2;

    t1.at<PLAN_ID>() = 1;
    t1.at<WHATEVER>() = foo(1, 2);
    t2.at<PLAN_ID>() = 2;
    t2.at<WHATEVER>() = foo(1, 2);
    ASSERT_TRUE(t1 < t2);
    EXPECT_EQ(-1, valcmp(t1, t2));

    t2.at<PLAN_ID>() = 1;
    t2.at<WHATEVER>() = foo(1, 3);
    ASSERT_TRUE(t1 < t2);
    EXPECT_EQ(-1, valcmp(t1, t2));

    //operator ==
    ASSERT_FALSE(t1 == t2);
    t2.at<WHATEVER>() = foo(1, 2);
    ASSERT_TRUE(t1 == t2);
    EXPECT_EQ(0, valcmp(t1, t2));

    //operator !=
    ASSERT_FALSE(t1 != t2);
    t2.at<WHATEVER>() = foo(2, 2);
    ASSERT_TRUE(t1 != t2);
    EXPECT_EQ(-1, valcmp(t1, t2));
}


// Test copy data from one tuple to another
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, basic_copy)
{
    typedef make_tuple<PLAN_ID, UNIT_ID, WHATEVER>::R	Tup1;
    struct RefStruct {
        PLAN_ID::Type plan_id;
        UNIT_ID::Type unit_id;
        WHATEVER::Type ever;
    };

    Tup1	t1, t2;

    t1.at<PLAN_ID>() = 100;
    t1.at<UNIT_ID>() = 10;
    t1.at<WHATEVER>() = foo(1, 2);

    //operator =
    t2 = t1;
    ASSERT_STREQ("(100 10 (1,2))", show(t2).c_str());
    EXPECT_EQ(sizeof(RefStruct), sizeof(t2));
}

DEFINE_ATTRIBUTE(FOO3, foo3);

// Test copy data from one tuple to another
// Date: 19/8/2010
// Author: zhuxingchang@baidu.com
TEST_F(test_usage_suite, string_copy)
{
    typedef make_tuple<PLAN_ID, UNIT_ID, FOO3>::R     Tup1;
    struct RefStruct {
        PLAN_ID::Type plan_id;
        UNIT_ID::Type unit_id;
        FOO3::Type foo3;
    };

    Tup1      t1, t2;
    
    t1.at<PLAN_ID>() = 10;
    t1.at<UNIT_ID>() = 20;
    t1.at<FOO3>() = foo3(3, "i love you!");
	
    ASSERT_EQ(sizeof(RefStruct), sizeof(t1));
    EXPECT_STREQ("(3,i love you!)", show(t1.at<FOO3>()).c_str());

    t2 = t1;
    ASSERT_EQ(sizeof(RefStruct), sizeof(t2));
    EXPECT_STREQ("(3,i love you!)", show(t2.at<FOO3>()).c_str());
	
}


