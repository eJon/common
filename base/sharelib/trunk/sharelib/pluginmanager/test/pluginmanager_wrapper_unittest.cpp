#include <gtest/gtest.h>
#include "../plugin_manager_wrapper.h"
#include <sharelib/test/test.h>
using namespace std;

using namespace sharelib;

TEST(PluginManagerWrapper, reload)
{
    PluginManagerWrapper wrapper;
    string configpath = string(string(TEST_DATA_PATH) + "/conf/");
    EXPECT_TRUE(0 == wrapper.Init(configpath));
    PluginManagerPtr managerOne = wrapper.GetPluginManager();
    system((string("touch ") + configpath + PluginManager::kPluginManagerConfFile).c_str());
    usleep(3000000);
    PluginManagerPtr managerTwo = wrapper.GetPluginManager();
    EXPECT_TRUE(managerOne.get() != managerTwo.get());
}

