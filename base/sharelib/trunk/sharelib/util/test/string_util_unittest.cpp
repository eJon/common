#include <gtest/gtest.h>
#include "../time_utility.h"
#include <sharelib/test/test.h>
#include <sharelib/util/file_util.h>
#include "../string_util.h"
using namespace std;

using namespace sharelib;

TEST(StringUtil, ReplaceAll)
{
    string content= "11111122222111111222222111111333333111111";
    string oldStr = "111111";
    string newStr = "444444";
    StringUtil::ReplaceAll(content, oldStr, newStr);
    cout<<"replace string:"<<content<<endl;
}

