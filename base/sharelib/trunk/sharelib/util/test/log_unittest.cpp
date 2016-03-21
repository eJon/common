#include <gtest/gtest.h>
#include "../string_util.h"
#include <sharelib/test/test.h>
#include <sharelib/util/log.h>
using namespace std;
using namespace sharelib;

TEST(StringUtility, readable)
{
    vector<uint32_t> dest_vec;
    StringUtil::Split("100,2,1000,  1", ",", dest_vec);
    for (size_t i = 0; i < dest_vec.size(); ++i) {
        cout<<"index:"<<i<<" value:"<<dest_vec[i]<<endl;
        LOG(LOG_LEVEL_DEBUG, Log4cppWrapper::GetLog("app"), "index : %d, value :%d",i , dest_vec[i]);
        LOG(LOG_LEVEL_DEBUG, Log4cppWrapper::GetLog("framework"), "index : %d, value :%d",i , dest_vec[i]);
        LOG(LOG_LEVEL_DEBUG, Log4cppWrapper::GetRootLog(), "index : %d, value :%d",i , dest_vec[i]);
        LOG_INITIAL(LOG_LEVEL_DEBUG, Log4cppWrapper::GetRootLog(), "index : %d, value :%d",i , dest_vec[i]);
        
        LOG_INITIAL(LOG_LEVEL_ERROR, Log4cppWrapper::GetRootLog(), "index : %d, value :%d",i , dest_vec[i]);
    }
}

