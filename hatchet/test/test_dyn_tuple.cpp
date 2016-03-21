// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Test dyn_tuple.h
// Author: gejun@baidu.com
// Date: Ò»  6ÔÂ 20 17:53:48 CST 2011

#include <gtest/gtest.h>
#include "dyn_tuple.h"
#include "named_tuple.hpp"
#include <stdarg.h>

using namespace st;

class test_usage_suite : public ::testing::Test{
protected:
    test_usage_suite(){};
    virtual ~test_usage_suite(){};
    virtual void SetUp() {
        srand(0);
    };
    virtual void TearDown() {
    };
};

TEST_F(test_usage_suite, endian)
{
    cout << "sizeof(std::string)=" << sizeof(std::string) << endl
         << "sizeof(Entry)=" << sizeof(DynTupleAccess) << endl;
    
    int64_t x = 0xABCDEFA123456789;
    char* buf = (char*)&x;
    cout << hex;
    cout << "x=" << x << " stored=";
    for (size_t i = 0; i < sizeof(x); ++i) {
        cout << (((int)buf[i]) & 0xFF);
    }
    cout << dec << endl;
}

TEST_F(test_usage_suite, copy)
{
    DynTupleSchema s1;
    s1.add_field("apple", FIELD_TYPE_INT, 16);
    s1.add_field("pear", FIELD_TYPE_UINT, 16);
    s1.add_field("orange", FIELD_TYPE_INT);

    DynTupleSchema s2;
    s2.add_field("apple", FIELD_TYPE_INT, 17);
    s2.add_field("pear", FIELD_TYPE_UINT, 16);
    
    cout << "s1=" << s1 << endl << "s2=" << s2 << endl;
    
    SCOPED_DYN_TUPLE(t1, &s1);
    t1.at_n(0) = 1;
    t1.at_n(1) = 2;
    t1.at_n(2) = 3;

    SCOPED_DYN_TUPLE(t2, &s1);
    t2 = t1;

    ASSERT_EQ(t1, t2);

    SCOPED_DYN_TUPLE(t3, &s2);
    t3 = t1;
    
    cout << "t1=" << t1 << " t2=" << t2 << " t3=" << t3 << endl;
}

TEST_F(test_usage_suite, comparison)
{
    DynTupleSchema s;
    s.add_field("apple", FIELD_TYPE_INT, 16);
    s.add_field("pear", FIELD_TYPE_UINT, 16);
    s.add_field("orange", FIELD_TYPE_INT, 32);

    DynTupleSchema s2;
    s2.add_field("apple", FIELD_TYPE_INT, 32);
    s2.add_field("pear", FIELD_TYPE_UINT, 32);
    
    ASSERT_EQ(s.field_num(), 3ul);
    ASSERT_EQ(s.byte_size(), 8ul);

    SCOPED_DYN_TUPLE(dt1, &s);
    SCOPED_DYN_TUPLE(dt2, &s);
    SCOPED_DYN_TUPLE(dt3, &s2);
    DynTuple dt4(NULL, NULL);
    DynTuple dt5(NULL, NULL);
    
    // isomorphic tuples
    for (int i = 0; i < 3; ++i) {
        dt1.at_n(0) = -2;
        dt1.at_n(1) = 2;
        dt1.at_n(2) = 3;
        dt2.at_n(0) = -2 + ((i % 3) == 0);
        dt2.at_n(1) = 2 + ((i % 3) == 1);
        dt2.at_n(2) = 3 + ((i % 3) == 2);
        
        ASSERT_LT(dt1, dt2);
        ASSERT_LE(dt1, dt2);
        ASSERT_GT(dt2, dt1);
        ASSERT_GE(dt2, dt1);
        ASSERT_NE(dt1, dt2);
        ASSERT_FALSE(dt1 == dt2);
    }

    // Incompatible tuples
    dt1.at_n(0) = 1;
    dt1.at_n(1) = 2u;
    dt1.at_n(2) = 3;
    dt3.at_n(0) = 1;
    dt3.at_n(1) = 2u;
        
    ASSERT_LT(dt3, dt1);
    ASSERT_LE(dt3, dt1);
    ASSERT_GT(dt1, dt3);
    ASSERT_GE(dt1, dt3);
    ASSERT_NE(dt1, dt3);
    ASSERT_FALSE(dt1 == dt3);

    dt1.at_n(0) = 1;
    dt1.at_n(1) = 2u;
    dt1.at_n(2) = 3;
    dt3.at_n(0) = 1;
    dt3.at_n(1) = 3u;
        
    ASSERT_LT(dt1, dt3);
    ASSERT_LE(dt1, dt3);
    ASSERT_GT(dt3, dt1);
    ASSERT_GE(dt3, dt1);
    ASSERT_NE(dt1, dt3);
    ASSERT_FALSE(dt1 == dt3);

    // null tuples
    ASSERT_FALSE(dt4 < dt5);
    ASSERT_FALSE(dt4 > dt5);
    ASSERT_GE(dt4, dt5);
    ASSERT_LE(dt4, dt5);
    ASSERT_EQ(dt4, dt5);
    ASSERT_FALSE(dt4 != dt5);
}

TEST_F(test_usage_suite, buggy_field_packing)
{
    DynTupleSchema s;
    s.add_field("apple", FIELD_TYPE_INT, 1);
    s.add_field("pear", FIELD_TYPE_INT, 31);
    s.add_field("orange", FIELD_TYPE_INT, 32);
    
    ASSERT_EQ(s.field_num(), 3ul);
    ASSERT_EQ(s.byte_size(), 8ul);
    
    SCOPED_DYN_TUPLE(dt, &s);
    dt.at_n(0) = 1;
    dt.at_n(1) = (int64_t)0xCD;
    dt.at_n(2) = 0xAB;

    ASSERT_EQ(-1, dt.at_n(0).to_int64());
    ASSERT_EQ(-1, dt.at_n(0).to_int32());
    ASSERT_EQ(0xCD, dt.at_n(1).to_int64());
    ASSERT_EQ(0xCD, dt.at_n(1).to_int32());
    ASSERT_EQ(0xAB, dt.at_n(2).to_int64());
    ASSERT_EQ(0xAB, dt.at_n(2).to_int32());
}

TEST_F(test_usage_suite, field_packing1)
{
    DynTupleSchema s;
    s.add_field("apple", FIELD_TYPE_INT, 16);
    s.add_field("pear", FIELD_TYPE_INT, 8);
    s.add_field("orange", FIELD_TYPE_INT, 8);
    s.add_field("banana", FIELD_TYPE_INT, 8);
    
    ASSERT_EQ(s.field_num(), 4ul);
    ASSERT_EQ(s.byte_size(), 8ul);
    
    DynTupleAccess n1(FIELD_TYPE_INT, 0, 0, 16, "apple");
    ASSERT_EQ(n1, *s.at_n(0));
    ASSERT_EQ(n1, *s.at("apple"));

    DynTupleAccess n2(FIELD_TYPE_INT, 0, 16, 8, "pear");
    ASSERT_EQ(n2, *s.at_n(1));
    ASSERT_EQ(n2, *s.at("pear"));

    DynTupleAccess n3(FIELD_TYPE_INT, 0, 24, 8, "orange");
    ASSERT_EQ(n3, *s.at_n(2));
    ASSERT_EQ(n3, *s.at("orange"));
    
    DynTupleAccess n4(FIELD_TYPE_INT, 0, 32, 8, "banana");
    ASSERT_EQ(n4, *s.at_n(3));
    ASSERT_EQ(n4, *s.at("banana"));

    ASSERT_EQ(&DynTupleAccess::EMPTY, s.at_n(4));
    ASSERT_EQ(&DynTupleAccess::EMPTY, s.at("orenge"));
}

TEST_F(test_usage_suite, field_packing2)
{
    DynTupleSchema s;
    ASSERT_EQ(0, s.add_field("apple", FIELD_TYPE_INT, 1));
    ASSERT_EQ(0, s.add_field("pear", FIELD_TYPE_INT, 1));
    ASSERT_EQ(0, s.add_field("orange", FIELD_TYPE_INT, 12));
    ASSERT_EQ(0, s.add_field("banana", FIELD_TYPE_INT, 10));
    ASSERT_EQ(0, s.add_field("watermelon", FIELD_TYPE_INT, 3));
    ASSERT_EQ(0, s.add_field("lemon", FIELD_TYPE_INT, 6));
    ASSERT_EQ(EEXIST, s.add_field("apple", FIELD_TYPE_INT, 6));
    
    ASSERT_EQ(s.field_num(), 6ul);
    ASSERT_EQ(s.byte_size(), 8ul);
    
    DynTupleAccess n1(FIELD_TYPE_INT, 0, 0, 1, "apple");
    ASSERT_EQ(n1, *s.at_n(0));
    ASSERT_EQ(n1, *s.at("apple"));

    DynTupleAccess n2(FIELD_TYPE_INT, 0, 1, 1, "pear");
    ASSERT_EQ(n2, *s.at_n(1));
    ASSERT_EQ(n2, *s.at("pear"));

    DynTupleAccess n3(FIELD_TYPE_INT, 0, 2, 12, "orange");
    ASSERT_EQ(n3, *s.at_n(2));
    ASSERT_EQ(n3, *s.at("orange"));

    DynTupleAccess n4(FIELD_TYPE_INT, 0, 14, 10, "banana");
    ASSERT_EQ(n4, *s.at_n(3));
    ASSERT_EQ(n4, *s.at("banana"));

    DynTupleAccess n5(FIELD_TYPE_INT, 0, 24, 3, "watermelon");
    ASSERT_EQ(n5, *s.at_n(4));
    ASSERT_EQ(n5, *s.at("watermelon"));

    DynTupleAccess n6(FIELD_TYPE_INT, 0, 27, 6, "lemon");
    ASSERT_EQ(n6, *s.at_n(5));
    ASSERT_EQ(n6, *s.at("lemon"));
    
    ASSERT_EQ(&DynTupleAccess::EMPTY, s.at_n(7));
    ASSERT_EQ(&DynTupleAccess::EMPTY, s.at("orenge"));
}

TEST_F(test_usage_suite, field_packing3)
{
    DynTupleSchema s;
    ASSERT_EQ(0, s.add_fields_by_string(" int16 apple "));
    ASSERT_EQ(EINVAL, s.add_fields_by_string("  "));
    ASSERT_EQ(EINVAL, s.add_fields_by_string(" int8 "));
    ASSERT_EQ(0, s.add_fields_by_string("int8 pear,  int8 orange,  int8 banana"));
    ASSERT_EQ(0, s.add_fields_by_string("double lemon"));
    ASSERT_EQ(EINVAL, s.add_fields_by_string("float63 lemon2"));
    SCOPED_DYN_TUPLE(dt, &s);
    dt.at("apple") = -1;
    dt.at("lemon") = 1.3;

    cout << "schema=" << s << endl
         << "tuple=" << dt << endl;
}

TEST_F(test_usage_suite, field_packing4)
{
    DynTupleSchema s1;
    ASSERT_EQ(0, s1.add_fields_by_string("int apple, int banana, int cherry"));
    ASSERT_EQ(12ul, s1.byte_size());

    DynTupleSchema s2;
    ASSERT_EQ(0, s2.add_fields_by_string("int64 apple, int banana"));
    ASSERT_EQ(12ul, s2.byte_size());
    cout << "*** FIXME ***" << endl;

    DynTupleSchema s3;
    ASSERT_EQ(0, s3.add_fields_by_string("int apple, int64 banana"));
    ASSERT_EQ(16ul, s3.byte_size());
}

TEST_F(test_usage_suite, mixed_types)
{
    DynTupleSchema s1;
    ASSERT_EQ(0, s1.add_fields_by_string(
                  "int64 apple, double banana, int cherry, float donut"));
    struct Ref {
        long apple;
        double banana;
        int cherry;
        float donut;
    };
    ASSERT_EQ(sizeof(Ref), s1.byte_size());
    SCOPED_DYN_TUPLE(t1, &s1);
    t1.at("apple") = 1;
    t1.at("banana") = 2.2;
    t1.at("cherry") = 3;
    t1.at("donut") = 4.4;

    Ref ref = { 1, 2.2, 3, 4.4 };
    ASSERT_EQ(0, memcmp(&ref, t1.buf(), sizeof(Ref)));
}

TEST_F(test_usage_suite, max_nfield)
{
    DynTupleSchema s1;
    ASSERT_EQ(0, s1.add_fields_by_string("int apple"));
    ASSERT_EQ(ERANGE, s1.add_fields_by_string("int banana", 0));
    ASSERT_EQ(0, s1.add_fields_by_string("int banana", 1));
    ASSERT_EQ(ERANGE, s1.add_fields_by_string("int cherry, int durian", 1));
    ASSERT_EQ(0, s1.add_fields_by_string("int cherry, int durian", 2));
}

TEST_F(test_usage_suite, many_fields)
{
    const int N = 500;
    DynTupleSchema s1;
    char name[32];
    for (int i = 0; i < N; ++i) {
        snprintf(name, sizeof(name), "%dapple", i);
        s1.add_field(name, FIELD_TYPE_INT);
    }
    ASSERT_EQ(N * sizeof(int), s1.byte_size());

    SCOPED_DYN_TUPLE(t1, &s1);
    for (int i = 0; i < N; ++i) {
        t1.at_n(i) = rand();
    }

    {
        int sum1 = 0;
        const int REP = 10000;
        TIME_SCOPE_IN_NS("DynTuple.at:", REP);
        for (int i = 0; i < REP; ++i) {
            sum1 += t1.at("198apple").to_int32();
        }
        cout << "sum1=" << sum1 << endl;
    }
}


TEST_F(test_usage_suite, field_write)
{
    DynTupleSchema s;
    ASSERT_EQ(
        0, s.add_fields_by_string(
            "int2 apple, int1 pear, int3 orange,"
            "int4 banana, int5 watermelon, uint64 lemon"));
    cout << "schema=" << s << endl;

    ASSERT_EQ(6ul, s.field_num());
    ASSERT_EQ(16ul, s.byte_size());

    SCOPED_DYN_TUPLE(t, &s);

    for (int i = -100; i < 100; ++i) {
        t.at("apple") = i;
        t.at("pear") = i;
        t.at("orange") = i;
        t.at("banana") = i;
        t.at("watermelon") = i;
        t.at("lemon") = (uint64_t)i;

        ASSERT_EQ(i & 3u, t.at("apple").to_uint32()) << "i=" << i;
        ASSERT_EQ(i & 1u, t.at("pear").to_uint32()) << "i=" << i;
        ASSERT_EQ(i & 7u, t.at("orange").to_uint32());
        ASSERT_EQ(i & 15u, t.at("banana").to_uint32());
        ASSERT_EQ(i & 31u, t.at("watermelon").to_uint32());
        ASSERT_EQ((uint64_t)i, t.at("lemon").to_uint64());
    }    
}

TEST_F(test_usage_suite, signed_integer)
{
    DynTupleSchema s;
    ASSERT_EQ(
        0, s.add_fields_by_string(
            "int8 apple, int8 banana, int16 cherry, int32 durian"));
    SCOPED_DYN_TUPLE(t, &s);
    t.at("apple") = 127;
    t.at("banana") = -128;
    t.at("cherry") = 32768;
    ASSERT_EQ(127, t.at("apple").to_int32());
    ASSERT_EQ(-128, t.at("banana").to_int32());
    ASSERT_EQ(-32768, t.at("cherry").to_int32());

    t.at("apple") = 128;
    t.at("banana") = -129;
    t.at("cherry") = -32769;
    ASSERT_EQ(-128, t.at("apple").to_int32());
    ASSERT_EQ(127, t.at("banana").to_int32());
    ASSERT_EQ(32767, t.at("cherry").to_int32());
}


TEST_F(test_usage_suite, set_by_value)
{
    DynTupleSchema s;
    ASSERT_EQ(
        0, s.add_fields_by_string("int32 apple, int16 banana, int16 cherry"));
    SCOPED_DYN_TUPLE(tup, &s);
    ASSERT_EQ(0, tup.set_by_string(" 16 , 18 , 20 "));
    ASSERT_EQ(0, tup.set_by_string("16, 18, 20"));
    ASSERT_EQ(0, tup.set_by_string("16,18,20"));
    ASSERT_EQ(0, tup.set_by_string("16 18 20", ' '));
    ASSERT_EQ(0, tup.set_by_string("16  18  20", ' '));
    ASSERT_EQ(16, tup.at("apple").to_int32());
    ASSERT_EQ(18, tup.at("banana").to_int32());
    ASSERT_EQ(20, tup.at("cherry").to_int32());


    SCOPED_DYN_TUPLE(tup2, &s);
    ASSERT_EQ(EINVAL, tup2.set_by_string("16, 18, ,20"));
    ASSERT_EQ(EINVAL, tup2.set_by_string("16, 18"));
    ASSERT_EQ(EINVAL, tup2.set_by_string("16, 18, 20, 22"));
    ASSERT_EQ(EINVAL, tup2.set_by_string("16a, 18, 20"));
}

struct NativeRef {
    int apple;
    int pear;
    int orange;
    int banana;
    int watermelon;
    u_int lemon;
};

TEST_F(test_usage_suite, rw_performance)
{
    const int N = 10000;
    DynTuple* dt_array = new DynTuple[N];
    DynTuple* dt_array2 = new DynTuple[N];
    NativeRef* ref_array = new NativeRef[N];

    DynTupleSchema s;
    ASSERT_EQ(0, s.add_fields_by_string("int64 apple, int64 banana, int64 cherry,"
                                        "int64 donut, uint lemon, int64 orange,"
                                        "int64 pear, int64 watermelon"));

    cout << "byte_size=" << s.byte_size() << " NativeRef=" << sizeof(NativeRef) << endl;

    char* data = new char[s.byte_size() * N];
    for (int i = 0; i < N; ++i) {
        dt_array[i].set_schema(&s);
        dt_array[i].set_buf(data + i * s.byte_size());
    }

    char* data2 = new char[s.byte_size() * N];
    for (int i = 0; i < N; ++i) {
        dt_array2[i].set_schema(&s);
        dt_array2[i].set_buf(data2 + i * s.byte_size());
    }
    
    cout << "<< Clear >>" << endl;
    {
        TIME_SCOPE_IN_NS("DynTuple.at_n:", N);
        for (int i = 0; i < N; ++i) {
            dt_array[i].at_n(0) = 0;
            dt_array[i].at_n(1) = 0;
            dt_array[i].at_n(2) = 0;
            dt_array[i].at_n(3) = 0;
            dt_array[i].at_n(4) = 0;
            dt_array[i].at_n(5) = 0;
        }
    }

    {
        TIME_SCOPE_IN_NS("DynTuple.at:", N);
        for (int i = 0; i < N; ++i) {
            dt_array2[i].at("apple") = 0;
            dt_array2[i].at("pear") = 0;
            dt_array2[i].at("orange") = 0;
            dt_array2[i].at("banana") = 0;
            dt_array2[i].at("watermelon") = 0;
            dt_array2[i].at("lemon") = 0;
        }
    }

    {
        TIME_SCOPE_IN_NS("NativeRef:", N);
        for (int i = 0; i < N; ++i) {
            ref_array[i].apple = 0;
            ref_array[i].pear = 0;
            ref_array[i].orange = 0;
            ref_array[i].banana = 0;
            ref_array[i].watermelon = 0;
            ref_array[i].lemon = 0;
        }
    }
    
    cout << "<< Write >>" << endl;
    {
        TIME_SCOPE_IN_NS("DynTuple.at_n:", N);
        for (int i = 0; i < N; ++i) {
            dt_array[i].at_n(0) = i;
            dt_array[i].at_n(1) = i+1;
            dt_array[i].at_n(2) = i+2;
            dt_array[i].at_n(3) = i+3;
            dt_array[i].at_n(4) = i+4;
            dt_array[i].at_n(5) = i+5;
        }
    }

    {
        TIME_SCOPE_IN_NS("DynTuple.at:", N);
        for (int i = 0; i < N; ++i) {
            dt_array2[i].at("apple") = i;
            dt_array2[i].at("pear") = i+1;
            dt_array2[i].at("orange") = i+2;
            dt_array2[i].at("banana") = i+3;
            dt_array2[i].at("watermelon") = i+4;
            dt_array2[i].at("lemon") = i+5;
        }
    }

    {
        TIME_SCOPE_IN_NS("NativeRef:", N);
        for (int i = 0; i < N; ++i) {
            ref_array[i].apple = i;
            ref_array[i].pear = i+1;
            ref_array[i].orange = i+2;
            ref_array[i].banana = i+3;
            ref_array[i].watermelon = i+4;
            ref_array[i].lemon = i+5;
        }
    }

    cout << "<< Read >>" << endl;
    int sum1 = 0, sum2 = 0, sum3 = 0;
    {
        TIME_SCOPE_IN_NS("DynTuple.at_n:", N);
        for (int i = 0; i < N; ++i) {
            sum1 += dt_array[i].at_n(0).to_int32();
            sum1 += dt_array[i].at_n(1).to_int32();
            sum1 += dt_array[i].at_n(2).to_int32();
            sum1 += dt_array[i].at_n(3).to_int32();
            sum1 += dt_array[i].at_n(4).to_int32();
            sum1 += dt_array[i].at_n(5).to_uint32();
        }
    }

    {
        TIME_SCOPE_IN_NS("DynTuple.at:", N);
        for (int i = 0; i < N; ++i) {
            sum2 += dt_array2[i].at("apple").to_int32();
            sum2 += dt_array2[i].at("pear").to_int32();
            sum2 += dt_array2[i].at("orange").to_int32();
            sum2 += dt_array2[i].at("banana").to_int32();
            sum2 += dt_array2[i].at("watermelon").to_int32();
            sum2 += dt_array2[i].at("lemon").to_uint32();
        }
    }

    {
        TIME_SCOPE_IN_NS("NativeRef:", N);
        for (int i = 0; i < N; ++i) {
            sum3 += ref_array[i].apple;
            sum3 += ref_array[i].pear;
            sum3 += ref_array[i].orange;
            sum3 += ref_array[i].banana;
            sum3 += ref_array[i].watermelon;
            sum3 += ref_array[i].lemon;
        }
    }
    ASSERT_EQ(sum1, sum2);
    ASSERT_EQ(sum1, sum3);
}


DEFINE_COLUMN(APPLE, int);
DEFINE_COLUMN(BANANA, int);
DEFINE_COLUMN(CHERRY, int);
DEFINE_COLUMN(DONUT, int);

TEST_F(test_usage_suite, comparison_speed_with_named_tuple)
{
    const int N = 50000;
    NaiveTimer tm;
    
    DynTupleSchema s;
    ASSERT_EQ(0, s.add_fields_by_string(
                  "int apple, int banana, int cherry, int donut"));
    char *buf = new char[N * s.byte_size()];
    DynTuple* ts1 = new DynTuple[N];
    for (int i = 0; i < N; ++i) {
        ts1[i].set_schema(&s);
        ts1[i].set_buf(buf + i * s.byte_size());
    }

    typedef NamedTuple<ST_MAKE_LIST(APPLE, BANANA, CHERRY, DONUT)> NT;
    NT* ts2 = new NT[N];


    ASSERT_EQ(sizeof(NT), s.byte_size());
    ASSERT_EQ(4ul, s.field_num());

    for (int i = 0; i < N; ++i) {
        ts2[i].at<APPLE>() = rand();
        ts1[i].at("apple") = ts2[i].at<APPLE>();
        
        ts2[i].at<BANANA>() = rand();
        ts1[i].at("banana") = ts2[i].at<BANANA>();
        
        ts2[i].at<CHERRY>() = rand();
        ts1[i].at("cherry") = ts2[i].at<CHERRY>();
        
        ts2[i].at<DONUT>() = rand();
        ts1[i].at("donut") = ts2[i].at<DONUT>();
    }

    size_t sum1 = 0, sum2 = 0;

    tm.start();
    for (int i = 0; i < N-1; ++i) {
        sum1 += (ts1[i] < ts1[i+1]);
    }
    tm.stop();
    cout << "operator-less-than: t1=" << tm.n_elapsed() / double(N-1) << "ns, ";
    
    tm.start();
    for (int i = 0; i < N-1; ++i) {
        sum2 += (ts2[i] < ts2[i+1]);
    }
    tm.stop();
    cout << "t2=" << tm.n_elapsed() / double(N-1) << "ns" << endl;
    ASSERT_EQ(sum1, sum2);


    tm.start();
    for (int i = 0; i < N-1; ++i) {
        sum1 += (ts1[i] == ts1[i+1]);
    }
    tm.stop();
    cout << "operator-equal: t1=" << tm.n_elapsed() / double(N-1) << "ns, ";
    
    tm.start();
    for (int i = 0; i < N-1; ++i) {
        sum2 += (ts2[i] == ts2[i+1]);
    }
    tm.stop();
    cout << "t2=" << tm.n_elapsed() / double(N-1) << "ns" << endl;
    ASSERT_EQ(sum1, sum2);

    for (int i = 0; i < N; ++i) {
        ASSERT_EQ(ts1[i].at_n(0).to_int64(), ts2[i].at<APPLE>());
        ASSERT_EQ(ts1[i].at_n(1).to_int64(), ts2[i].at<BANANA>());
        ASSERT_EQ(ts1[i].at_n(2).to_int64(), ts2[i].at<CHERRY>());
        ASSERT_EQ(ts1[i].at_n(3).to_int64(), ts2[i].at<DONUT>());
    }

    tm.start();
    for (int i = 0; i < N; ++i) {
        sum1 ^= hash(ts1[i]);
    }
    tm.stop();
    cout << "hash-code: t1=" << tm.n_elapsed() / double(N) << "ns, ";

    tm.start();
    for (int i = 0; i < N; ++i) {
        sum2 ^= hash(ts2[i]);
    }
    tm.stop();
    cout << "t2=" << tm.n_elapsed() / double(N) << "ns" << endl;
    ASSERT_EQ(sum1, sum2);


    delete [] ts1;
    delete [] ts2;
}

