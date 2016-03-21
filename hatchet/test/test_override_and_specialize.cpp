// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2011-01-14 06:17

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_basic_suite : public ::testing::Test{
protected:
    test_basic_suite(){};
    virtual ~test_basic_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

void foo (int) {}
template <typename> struct Goo {};


void by_override (int)
{
    cout << "int";
}

void by_override (float)
{
    cout << "float";
}

template <typename _T1, typename _T2>
void by_override(_T1(_T2))
{
    cout << "T1(T2)";
}

template <typename _T>
void by_override(_T)
{
    cout << "T";
}

template <typename _T>
void by_override(Goo<_T>)
{
    cout << "Goo<";
    _T d;
    by_override(d);
    cout << ">";
}

// -----------------
template <typename _T>
struct by_specialize {
    static void call () {
        cout << "T";
    }
};

template <>
struct by_specialize<int> {
    static void call() {
        cout << "int";
    }
};

template <typename _T1, typename _T2>
struct by_specialize<_T1(_T2)> {
    static void call() {
        cout << "T1(T2)";
    }
};

template <typename _T>
struct by_specialize<Goo<_T> > {
    static void call() {
        cout << "Goo<";
        by_specialize<_T>::call();
        cout << ">";
    }
};


// -------------------

TEST_F(test_basic_suite, call_and_see)
{
    by_override(1); cout << endl;
    by_override(foo); cout << endl;
    // by_override(Goo<typeof(foo)>()); cout << endl;
    by_specialize<int>::call(); cout << endl;
    by_specialize<typeof(foo)>::call(); cout << endl;
    by_specialize<Goo<typeof(foo)> >::call(); cout << endl;
}
