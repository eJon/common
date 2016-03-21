// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-12-20 09:13

#include <gtest/gtest.h>
#include "compare.hpp"
#include "array_op.hpp"

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

//! to check array_binary_search
template <typename _CompareXY, typename _X, typename _Y>
inline int array_linear_search (const _X& item, const _Y* a_y, int size)
{
    _CompareXY cmp;
    for (int i=0; i<size; ++i) {
        int r = cmp(item, a_y[i]);
        if (0 == r) {
            return i;
        }
        else if (r < 0) {
            return ~i;
        }
    }
    return ~size;
}

TEST_F(test_usage_suite, binary_search)
{
    const int MAXLEN = 40;
    int buf[MAXLEN];

    for(int k = 0; k < 100; ++k) {
        for(int i = 0; i < MAXLEN; ++i) {
            buf[i] = rand();
        }
        std::sort(buf, buf + MAXLEN);

        for(int j = 0; j < 1000; ++j) {
            for(int i = 5; i < MAXLEN; i++) {
                int value = rand();
                ASSERT_EQ(array_binary_search<Compare<int> > (value, buf, i, 32)
                        , array_linear_search<Compare<int> > (value, buf, i));
                //<< "input=" << value << " 0=" << buf[0] << ",1=" << buf[1];
            }
        }
    }
}

TEST_F(test_usage_suite, array_intersected_backward_copy)
{
    const int LEN = 10;
    int a[LEN*3];
    for (int i=0; i<LEN*3; ++i) {
        a[i] = i;
    }
    array_copy (a+LEN-1, a+LEN, a+2*LEN);
    for (int i=0; i<LEN-1; ++i) {
        ASSERT_EQ (a[i], i);
    }
    for (int i=LEN-1; i<2*LEN-1; ++i) {
        ASSERT_EQ (a[i], i+1);
    }
    for (int i=2*LEN-1; i<3*LEN; ++i) {
        ASSERT_EQ (a[i], i);
    }
}

TEST_F(test_usage_suite, array_intersected_forward_copy)
{
    const int LEN = 10;
    int a[LEN*3];
    for (int i=0; i<LEN*3; ++i) {
        a[i] = i;
    }
    array_copy (a+LEN+1, a+LEN, a+2*LEN);
    for (int i=0; i<LEN+1; ++i) {
        ASSERT_EQ (a[i], i);
    }
    for (int i=LEN+1; i<2*LEN+1; ++i) {
        ASSERT_EQ (a[i], i-1);
    }
    for (int i=2*LEN+1; i<3*LEN; ++i) {
        ASSERT_EQ (a[i], i);
    }    
}

TEST_F(test_usage_suite, array_in_place_erase)
{
    const int LEN = 10;
    int a[LEN];
    for (int j=0; j<LEN; ++j) {
        for (int i=0; i<LEN; ++i) {
            a[i] = i;
        }
        ASSERT_TRUE (array_erase (a, a+LEN, a+j));

        for (int i=0; i<j; ++i) {
            ASSERT_EQ (a[i], i);
        }
        for (int i=j; i<LEN-1; ++i) {
            ASSERT_EQ (a[i], i+1);
        }
    }
}

TEST_F(test_usage_suite, array_not_in_place_erase)
{
    const int LEN = 10;
    int a[LEN];
    int b[LEN];
    for (int j=0; j<LEN-1; ++j) {
        for (int i=0; i<LEN; ++i) {
            a[i] = i;
            b[i] = i;
        }
        ASSERT_TRUE(array_nip_erase (b, a, a+LEN, a+j));
        for (int i=0; i<j; ++i) {
            ASSERT_EQ (b[i], i);
        }
        for (int i=j; i<LEN-1; ++i) {
            ASSERT_EQ (b[i], i+1);
        }
        for (int i=0; i<LEN; ++i) {
            ASSERT_EQ (a[i], i);
        }
    }
}

TEST_F(test_usage_suite, array_not_in_place_intersected_erase)
{
    const int LEN = 10;
    int c[LEN*3];
    for (int j=0; j<=LEN*2; ++j) {
        for (int k=LEN; k<LEN*2; ++k) {
            for (int i=0; i<LEN*3; ++i) {
                c[i] = i;
            }
            ASSERT_TRUE(array_nip_erase (c+j, c+LEN, c+2*LEN, c+k));
            for (int i=0; i<j; ++i) {
                ASSERT_EQ (c[i], i);
            }
            for (int i=j; i<j+k-LEN; ++i) {
                ASSERT_EQ (c[i], i+LEN-j);
            }
            for (int i=j+k-LEN; i<j+LEN-1; ++i) {
                ASSERT_EQ (c[i], i+LEN-j+1);
            }
            for (int i=j+LEN-1; i<LEN*3; ++i) {
                ASSERT_EQ (c[i], i);
            }
        }
    }
}


TEST_F(test_usage_suite, array_in_place_insert)
{
    const int LEN = 10;
    int a[LEN+1];
    int magic = 1234;
    for (int j=0; j<LEN; ++j) {
        for (int i=0; i<LEN+1; ++i) {
            a[i] = i;
        }
        ASSERT_TRUE (array_insert (a, a+LEN, a+j, magic));
        for (int i=0; i<j; ++i) {
            ASSERT_EQ (a[i], i);
        }
        ASSERT_EQ (a[j], magic);
        for (int i=j+1; i<LEN+1; ++i) {
            ASSERT_EQ (a[i], i-1);
        }
    }
}

TEST_F(test_usage_suite, array_not_in_place_insert)
{
    const int LEN = 10;
    int a[LEN+1];
    int b[LEN+1];
    int magic = 1234;
    for (int j=0; j<LEN; ++j) {
        for (int i=0; i<LEN+1; ++i) {
            a[i] = i;
            b[i] = i;
        }
        ASSERT_TRUE(array_nip_insert (b, a, a+LEN, a+j, magic));
        for (int i=0; i<j; ++i) {
            ASSERT_EQ (b[i], i);
        }
        ASSERT_EQ (b[j], magic);
        for (int i=j+1; i<LEN+1; ++i) {
            ASSERT_EQ (b[i], i-1);
        }
        for (int i=0; i<LEN+1; ++i) {
            ASSERT_EQ (a[i], i);
        }
    }
}

TEST_F(test_usage_suite, array_not_in_place_intersected_insert)
{
    const int LEN = 10;
    int c[LEN*3+1];
    int magic = 1234;

    for (int j=0; j<=LEN*2; ++j) {
        for (int k=LEN; k<LEN*2; ++k) {
            for (int i=0; i<LEN*3+1; ++i) {
                c[i] = i;
            }
            ASSERT_TRUE(array_nip_insert (c+j, c+LEN, c+2*LEN, c+k, magic));
            // cout << "j=" << j << " k=" << k << ":";
            // for (int i=0; i<LEN*3+1; ++i) {
            //     cout << " (" << i << "," << c[i] << ")";
            // }
            // cout << endl;
            for (int i=0; i<j; ++i) {
                ASSERT_EQ (c[i], i);
            }
            for (int i=j; i<j+k-LEN; ++i) {
                ASSERT_EQ (c[i], i-j+LEN);
            }
            ASSERT_EQ (c[j+k-LEN], magic);
            for (int i=j+k-LEN+1; i<j+LEN+1; ++i) {
                ASSERT_EQ (c[i], i-j+LEN-1);
            }
            for (int i=j+LEN+1; i<LEN*3+1; ++i) {
                ASSERT_EQ (c[i], i);
            }
        }
    }
}
