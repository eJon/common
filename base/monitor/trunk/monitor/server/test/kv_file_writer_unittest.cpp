#include <gtest/gtest.h>

#include <sharelib/util/time_utility.h>

#include <monitor/test/test.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
#include <monitor/test/test.h>
#include <monitor/server/kv_file_writer.h>
using namespace std;
using namespace monitor;
using namespace sharelib;
TEST(KvFileWriter, Add)
{
    MonitorInfoPtr info(new MonitorInfo());
    info->content = "|a_1:10|a_2:20|a_3:30|a_4:0|";
    
    
    SingleWritePattern pattern1;
    pattern1.sourceFields.push_back("a_1");
    pattern1.sourceFields.push_back("a_2");
    pattern1.sourceFields.push_back("a_3");
    pattern1.destField = "b_1";
    pattern1.calculation = "+";
    
    AppConfPtr appconf(new AppConf);
    appconf->patterns.push_back(pattern1);
    KvFileWriter writer;
    MonitorInfoPtr newInfo = writer.NormalizeInfo(info, appconf);
    EXPECT_TRUE(newInfo->content == "|a_1:10|a_2:20|a_3:30|a_4:0|b_1:60|");
}


TEST(KvFileWriter, sub)
{
    MonitorInfoPtr info(new MonitorInfo());
    info->content = "|a_1:10|a_2:20|a_3:30|a_4:0|";
    
    
    SingleWritePattern pattern1;
    pattern1.sourceFields.push_back("a_2");
    pattern1.sourceFields.push_back("a_1");
    pattern1.destField = "b_1";
    pattern1.calculation = "-";
    
    AppConfPtr appconf(new AppConf);
    appconf->patterns.push_back(pattern1);
    KvFileWriter writer;
    MonitorInfoPtr newInfo = writer.NormalizeInfo(info, appconf);
    EXPECT_TRUE(newInfo->content == "|a_1:10|a_2:20|a_3:30|a_4:0|b_1:10|");
}

TEST(KvFileWriter, mul)
{
    MonitorInfoPtr info(new MonitorInfo());
    info->content = "|a_1:10|a_2:20|a_3:30|a_4:0|";
    
    
    SingleWritePattern pattern1;
    pattern1.sourceFields.push_back("a_1");
    pattern1.sourceFields.push_back("a_2");
    pattern1.destField = "b_1";
    pattern1.calculation = "*";
    
    AppConfPtr appconf(new AppConf);
    appconf->patterns.push_back(pattern1);
    KvFileWriter writer;
    MonitorInfoPtr newInfo = writer.NormalizeInfo(info, appconf);
    EXPECT_TRUE(newInfo->content == "|a_1:10|a_2:20|a_3:30|a_4:0|b_1:200|");
}

TEST(KvFileWriter, devid)
{
    MonitorInfoPtr info(new MonitorInfo());
    info->content = "|a_1:10|a_2:20|a_3:30|a_4:0|";
    
    
    SingleWritePattern pattern1;
    pattern1.sourceFields.push_back("a_2");
    pattern1.sourceFields.push_back("a_1");
    pattern1.destField = "b_1";
    pattern1.calculation = "/";
    
    AppConfPtr appconf(new AppConf);
    appconf->patterns.push_back(pattern1);
    KvFileWriter writer;
    MonitorInfoPtr newInfo = writer.NormalizeInfo(info, appconf);
    EXPECT_TRUE(newInfo->content == "|a_1:10|a_2:20|a_3:30|a_4:0|b_1:2|");
}
