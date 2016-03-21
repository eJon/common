#include <gtest/gtest.h>

#include <sharelib/util/time_utility.h>

#include <monitor/test/test.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
#include <monitor/test/test.h>
#include <monitor/server/ip_file_writer.h>
using namespace std;
using namespace monitor;
using namespace sharelib;
TEST(ipfilewritertest, write)
{
    MonitorInfoPtr info1(new MonitorInfo());
    info1->content = "|a_1:10|a_2:20|a_3:30|a_4:0|";
    info1->ip="ip1";
    
    MonitorInfoPtr info2(new MonitorInfo());
    info2->content = "|a_1:20|a_2:30|a_3:40|a_4:0|";
    info2->ip="ip2";
    
    string iptestDir = string(TEST_DATA_PATH) + "/iptest/";
    system((string("mkdir " + iptestDir).c_str()));
    AppConfPtr appconf(new AppConf);
    SingleWritePattern pattern1;
    pattern1.sourceFields.push_back("a_1");
    pattern1.sourceFields.push_back("a_2");
    pattern1.sourceFields.push_back("a_3");
    pattern1.destField = "b_1";
    pattern1.calculation = "+";
    
    appconf->patterns.push_back(pattern1);
    IpFileWriter writer;
    EXPECT_TRUE(0 == writer.Init(appconf, iptestDir));
    
    writer.Write(info1);
    writer.Write(info2);
    system((string("rm -rf " + iptestDir).c_str()));
}
