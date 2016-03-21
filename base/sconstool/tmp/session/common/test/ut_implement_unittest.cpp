#include <session/common/ut_implement.h>
#include <errno.h>
#include <assert.h>
#include <gtest/gtest.h>

using namespace std;
using namespace session;


TEST(Lock, test1) { 
    UtImplement ut;
    EXPECT_TRUE(1 ==ut.Get());

}
TEST(Lock, test2) { 
    UtImplement ut;
    EXPECT_TRUE(1 ==ut.Get());
}
