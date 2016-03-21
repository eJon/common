#include <gtest/gtest.h>
#include <sharelib/server/tcp_client.h>
#include <sharelib/util/time_utility.h>
#include <sharelib/server/epoll_server.h>
#include <monitor/test/test.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
#include <monitor/test/test.h>
#include <monitor/server/test/server_test_tool.h>
#include <monitor/server/app_conf_manager.h>
using namespace std;
using namespace monitor;
using namespace sharelib;
TEST(AppConfManager, MultiWrite)
{
    
    string appName1 = "app_test1";
    string appName2 = "app_test2";
    string parent= "app_conf_manager_multiwrite";
    string appConfDir = string(TEST_DATA_PATH) + parent + "/conf";
    system((string("mkdir -p ") + appConfDir).c_str());
        
    string appWriteDir1 = string(TEST_DATA_PATH) + parent + "/data/" + appName1;
    string appWriteDir2 = string(TEST_DATA_PATH) + parent + "/data/" + appName2;
    AppConfPtr appConf1 = ServerTestTool::GenerateAppconf(appWriteDir1);
    AppConfPtr appConf2 = ServerTestTool::GenerateAppconf(appWriteDir2);
    AppConf::WriteToFile(appConf1, appConfDir + "/" + appName1);
    AppConf::WriteToFile(appConf2, appConfDir + "/" + appName2);
    
    
    AppConfManagerPtr appConfManager(new AppConfManager);
    appConfManager->SetReloadSpan(1);
    EXPECT_TRUE(0 == appConfManager->Init(appConfDir));
    sleep(1);
    uint32_t sendCount = 10;
    for(uint32_t i = 0;i < sendCount;i++){
        stringstream ss;
        ss << i;
        MonitorInfoPtr info1 = ServerTestTool::GenerateMonitorInfo(appName1, ss.str());
        MonitorInfoPtr info2 = ServerTestTool::GenerateMonitorInfo(appName2, ss.str());
        appConfManager->Write(info1);
        appConfManager->Write(info2);
    }
    for(uint32_t i = 0;i < sendCount;i++){
        stringstream ss;
        ss << i;
        MonitorInfoPtr info1 = ServerTestTool::GenerateMonitorInfo(appName1, "cumulation" ,ss.str());
        MonitorInfoPtr info2 = ServerTestTool::GenerateMonitorInfo(appName2, "cumulation" ,ss.str());
        appConfManager->Write(info1);
        appConfManager->Write(info2);
    }
    
    system((string("rm -rf ") + string(TEST_DATA_PATH) + parent).c_str());
}

TEST(AppConfManager, BasicFunctionAndReload)
{
    string appName = "app_test";
    string parent= "app_conf_manager_test";
    string appConfDir = string(TEST_DATA_PATH) + parent + "/conf";
    system((string("mkdir -p ") + appConfDir).c_str());
        
    string appWriteDir = string(TEST_DATA_PATH) + parent + "/data";
    
    AppConfPtr appConf = ServerTestTool::GenerateAppconf(appWriteDir);
    appConf->WriteToFile(appConf, appConfDir + "/" + appName);
    
    
    AppConfManagerPtr appConfManager(new AppConfManager);
    appConfManager->SetReloadSpan(1);
    EXPECT_TRUE(0 == appConfManager->Init(appConfDir));
    sleep(1);
    uint32_t sendCount = 10;
    for(uint32_t i = 0;i < sendCount;i++){
        stringstream ss;
        ss << i;
        MonitorInfoPtr info = ServerTestTool::GenerateMonitorInfo(appName, ss.str());
        appConfManager->Write(info);
    }
    system((string("touch ") + appConfDir).c_str());
    sleep(3);
    while(appConfManager->GetReloadCount() != 1){
        usleep(100000);
        MONITOR_LOG_ERROR("wait reload");
    }
    for(uint32_t i = 0;i < sendCount;i++){
        stringstream ss;
        ss << i;
        MonitorInfoPtr info = ServerTestTool::GenerateMonitorInfo(appName, "cumulation" ,ss.str());
        appConfManager->Write(info);
    }
    
    system((string("rm -rf ") + string(TEST_DATA_PATH) + parent).c_str());
}


