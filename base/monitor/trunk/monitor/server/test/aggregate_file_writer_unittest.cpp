#include <gtest/gtest.h>
#include <sharelib/util/time_utility.h>
#include <monitor/test/test.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
#include <monitor/test/test.h>
#include <monitor/server/aggregate_file_writer.h>
using namespace std;
using namespace monitor;
using namespace sharelib;
TEST(AggregateFileWriter, Normalize)
{
    vector<MonitorInfoWithTime> infos;
    AppConfPtr appconf(new AppConf);

    MonitorInfoPtr info1(new MonitorInfo());
    info1->content = "|a_1:10|a_2:20|a_3:30|a_4:0|";
    info1->ip = "ip1";
    info1->threadid = 1;
    MonitorInfoWithTime st1(info1, 0);
    infos.push_back(st1);
    
    MonitorInfoPtr info2(new MonitorInfo());
    info2->content = "|a_1:20|a_2:30|a_3:40|a_4:1|";
    info2->ip = "ip2";
    info2->threadid = 2;
    MonitorInfoWithTime st2(info2, 0);
    infos.push_back(st2);
    

    SingleWritePattern pattern1;
    pattern1.sourceFields.push_back("a_1");
    pattern1.sourceFields.push_back("a_2");
    pattern1.sourceFields.push_back("a_3");
    pattern1.destField = "b_1";
    pattern1.calculation = "+";
    SingleWritePattern pattern2;
    pattern2.sourceFields.push_back("b_1");
    pattern2.sourceFields.push_back("a_1");
    pattern2.destField = "b_2";
    pattern2.calculation = "/";
    appconf->patterns.push_back(pattern1);
    appconf->patterns.push_back(pattern2);
    
    AggregateFileWriter writer;
    MonitorInfoPtr newInfo = writer.NormalizeInfo(infos, appconf);
    cout << "new content " << newInfo->content << endl;
    EXPECT_TRUE(newInfo->content == "|a_1:30|a_2:50|a_3:70|a_4:1|b_1:150|b_2:5|groupCount:2|");
}

