#include <gtest/gtest.h>
#include <map>
#include <sharelib/util/time_utility.h>
#include <sharelib/util/file_util.h>
#include <sharelib/util/filekv_parser.h>
#include <sharelib/test/test.h>
using namespace std;

using namespace sharelib;


TEST(tmp, parseFake)
{
    string file = string(TEST_DATA_PATH) + "kv.test";
    FilekvParser parser;
    std::map<string,string> maps;
    EXPECT_TRUE(parser.Parse(file));
    bool isExist;
    bool isConvert;
    EXPECT_TRUE(parser.GetInt64("int", isExist, isConvert) == 1);
    EXPECT_TRUE(isExist && isConvert);
    
    parser.GetInt64("invalidint", isExist, isConvert);;
    EXPECT_TRUE(isExist);
    EXPECT_TRUE(!isConvert);

    string str = parser.GetStr("string", isExist);;
    EXPECT_TRUE(str == "str");
    EXPECT_TRUE(isExist = true);

}
