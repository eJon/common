#include <gtest/gtest.h>
#include <sharelib/json/jsonizable.h>
#include <sharelib/test/test.h>

using namespace std;
SHARELIB_US;

struct Example : Jsonizable
{
    int data;
    Example() {}
    Example(int data) : data(data) {}
    void Jsonize(Jsonizable::JsonWrapper& json)
    {
        json.Jsonize("data", data);
    }
};

struct Example2 : Jsonizable
{
    std::string b1, b2;
    Example2(std::string b1, std::string b2) : b1(b1), b2(b2)
    {}
        
    void Jsonize(Jsonizable::JsonWrapper& json)
    {
        json.JsonizeBinary("binary1", b1);
        json.JsonizeBinary("binary2", b2, "\001\002\003");
    }
};

TEST(TestSimpleProcess, json)
{ 
    Example *e = new Example(1);
    std::string str = ToJsonString(e);
    Example *f = NULL;
    FromJsonString(f, str);
    cout << "f is great" << endl;
    EXPECT_EQ(f->data, 1);
    
    Example g;
    FromJsonString(g, str);
    cout << "g is great" << endl;
    EXPECT_EQ(g.data, 1);
    
    
    delete e;
    delete f;
}

TEST(TestJsonizeBinary, json)
{
    Example2 withBinary("\xFF\xFF\xFF\xFF\xFF\xFF\xFF", "123");
    Example2 back("", "");
    std::string s = ToJsonString<Example2>(withBinary);
    std::string cs = ToJsonStringCompact<Example2>(withBinary); 
    cout << "json is " << s <<endl;
    cout << "compact json is " << cs <<endl;
    FromJsonString<Example2>(back, cs);

    EXPECT_EQ(back.b1, string("\xFF\xFF\xFF\xFF\xFF\xFF\xFF"));
    EXPECT_EQ(back.b2, string("123"));
}


TEST(TestJsonizeChz, json)
{
    Example2 withBinary("中国", "123");
    Example2 back("", "");
    std::string s = ToJsonString<Example2>(withBinary);
    FromJsonString<Example2>(back, s);

    EXPECT_EQ(back.b1, string("中国"));
    EXPECT_EQ(back.b2, string("123"));
}
TEST(TestJsonMap, json)
{
    JsonMap jm;
    FromJsonString(jm, "{\"aaa\":\"xxx\"}");
    EXPECT_EQ(string("xxx"), AnyCast<std::string>(jm["aaa"]));

    FromJsonString(jm, "{\"aaa\":34}");
    EXPECT_EQ(int(34), JsonNumberCast<int>(jm["aaa"]));
    
}


