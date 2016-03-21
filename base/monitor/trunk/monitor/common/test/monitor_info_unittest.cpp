#include <gtest/gtest.h>
#include <monitor/test/test.h>
#include <monitor/common/monitor_info.h>
#include <monitor/common/log.h>
#include <sharelib/json/string_tools.h>
using namespace std;
using namespace sharelib;
using namespace monitor;
class FakeInfo : public sharelib::Jsonizable
{
public:
    void Jsonize(sharelib::Jsonizable::JsonWrapper& json){
        json.Jsonize("a", a);
        json.Jsonize("b", b);
    }
public:
    int a;
    std::string b;
};
TEST(MonitorInfo, stringquote)
{
    
    MonitorInfo info;
    
    FakeInfo fake;
    fake.a = 10;
    fake.b = "ggg\"";
    string fs = ToJsonStringCompact(fake);
    info.content = fs;

    MonitorInfoPtr infoPtr(new MonitorInfo(info));
    string infos = ToJsonStringCompact(infoPtr);
    MONITOR_LOG_ERROR("appconfmanager write eror %s", infos.c_str());
    cout << "infos is " << infos << endl;

    MonitorInfo *infoGene= NULL;
    FromJsonString(infoGene, infos);
    FakeInfo *fakeGene =NULL;
    FromJsonString(fakeGene, infoGene->content);
    
    cout << "fake from " << infoGene->content << endl;
    EXPECT_TRUE(fakeGene->a == 10);
    EXPECT_TRUE(fakeGene->b == "ggg\"");
    
    delete fakeGene;
    delete infoGene;
}

