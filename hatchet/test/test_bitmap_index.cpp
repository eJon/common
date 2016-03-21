
#include <gtest/gtest.h>
#include "cow_table.hpp"
#include "st_timer.h"
#include <ext/hash_map>
#include "bitmap_index.hpp"

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

TEST_F(test_basic_suite, bitmap_index)
{
    typedef NamedTuple<ST_MAKE_LIST(UNIT_ID, PLAN_ID, USER_ID)> Tup;
    typedef BitmapIndex<Tup, Cons<UNIT_ID, void> > BI;

    BI bi;
    bi.init(1000);
    
    bi.insert(Tup(1, 1, 1));
    bi.insert(Tup(1, 10, 10));
    bi.insert(Tup(2, 2, 2));
    bi.insert(Tup(3, 3, 3));
    cout << "bi:" << endl;
    for (BI::Iterator it = bi.begin(); it != bi.end(); ++it) {
        cout << *it << endl;
    }
    EXPECT_EQ(3ul, bi.size());

    bi.erase_tuple(Tup(2,2,2));
    cout << "after erase bi:" << endl;
    for (BI::Iterator it = bi.begin(); it != bi.end(); ++it) {
        cout << *it << endl;
    }
    EXPECT_EQ(2ul, bi.size());
  
    BI bi2(bi);
    bi2.insert(Tup(6, 10, 10));
    bi2.insert(Tup(8, 2, 2));
    bi2.insert(Tup(11, 3, 3));
    cout << "after copy bi and insert, bi2:" << endl;
    for (BI::Iterator it = bi2.begin(); it != bi2.end(); ++it) {
        cout << *it << endl;
    }
    
    
    EXPECT_FALSE(bi2.not_init());
    EXPECT_FALSE(bi2.empty());
    EXPECT_EQ(5ul, bi2.size());  

    bi2.resize(1024);
    EXPECT_EQ(5ul, bi2.size());  
    cout << "after resize bi2:" << endl;
    for (BI::Iterator it = bi2.begin(); it != bi2.end(); ++it) {
        cout << *it << endl;
    }
    
    
    bi.clear();
    cout << "after clear bi:" << endl;
    for (BI::Iterator it = bi.begin(); it != bi.end(); ++it) {
        cout << *it << endl;
    }
}

