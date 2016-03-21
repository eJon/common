#include <gtest/gtest.h>

#include <sharelib/util/time_utility.h>

#include <monitor/test/test.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
#include <monitor/test/test.h>
#include <monitor/server/mixed_app_writer.h>
using namespace std;
using namespace monitor;
using namespace sharelib;
TEST(mixedfilewritertest, write)
{
    MonitorInfoPtr info1(new MonitorInfo());
    info1->content = "|a_1:10|a_2:20|a_3:30|a_4:0|";
    info1->ip="ip1";
    info1->dataid = "default";
    
    MonitorInfoPtr info2(new MonitorInfo());
    info2->content = "|a_1:20|a_2:30|a_3:40|a_4:0|";
    info2->ip="ip2";
    info2->dataid=CUMULATION_DATAID;
    
    string mixedtestDir = string(TEST_DATA_PATH) + "/mixedtest/";
    system((string("mkdir " + mixedtestDir).c_str()));
    AppConfPtr appconf(new AppConf);
    SingleWritePattern pattern1;
    pattern1.sourceFields.push_back("a_1");
    pattern1.sourceFields.push_back("a_2");
    pattern1.sourceFields.push_back("a_3");
    pattern1.destField = "b_1";
    pattern1.calculation = "+";
    
    appconf->maxFileCount = 2;
    appconf->writePath = mixedtestDir;
    appconf->maxFileTime  = 1;
    appconf->aggrSpanTime = 10;
    appconf->patterns.push_back(pattern1);
    MixedAppWriter* writer = new MixedAppWriter();
    EXPECT_TRUE(0 == writer->Init(appconf, mixedtestDir));
    for(uint32_t i = 0;i < 10;i++){
        writer->Write(info1);
        writer->Write(info2);
    }
    sleep(1);
    for(uint32_t i =0;i<10;i++){
        writer->Write(info1);
        writer->Write(info2);
    }
    delete writer;
    EXPECT_TRUE(FileUtil::IsFileExist(mixedtestDir + "raw.log"));
    EXPECT_TRUE(FileUtil::IsFileExist(mixedtestDir + "raw.log.1"));
    EXPECT_TRUE(FileUtil::IsFileExist(mixedtestDir + "ip2"));
    EXPECT_TRUE(FileUtil::IsFileExist(mixedtestDir + "ip2.1"));
    EXPECT_TRUE(FileUtil::IsFileExist(mixedtestDir + "snapshot.log"));
    EXPECT_TRUE(FileUtil::IsFileExist(mixedtestDir + "snapshot.log.1"));
    EXPECT_TRUE(FileUtil::IsFileExist(mixedtestDir + "aggr.log"));
    EXPECT_TRUE(FileUtil::IsFileExist(mixedtestDir + "aggr.log.1"));
//    system((string("rm -rf " + mixedtestDir).c_str()));
}
