#include <gtest/gtest.h>
#include "engine_common/Log_r.h"
#include "../plugin_manager.h"
#include <sharelib/pluginmanager/handler_iplugin.h>
#include <sharelib/test/test.h>
using namespace std;

using namespace sharelib;

class PluginManagerTest:public testing::Test
{
  protected:
    static void SetUpTestCase()
    {
      plugin_manager_test_ = new PluginManager();
    }
    static void TearDownTestCase()
    {
      delete plugin_manager_test_;
    }
    static PluginManager* plugin_manager_test_;
};

PluginManager* PluginManagerTest::plugin_manager_test_= NULL;

TEST_F(PluginManagerTest, InitTest)
{
  string config_file = string(string(TEST_DATA_PATH) + "/conf/");
  int ret = plugin_manager_test_->Init(config_file.c_str());
  EXPECT_EQ(0, ret);
}

TEST_F(PluginManagerTest, LoadPluginTest)
{
  string plugin_name, plugin_home;
  PluginInfoPtr plugin_info_ptr(new PluginInfo);
  plugin_info_ptr->plugin_conf.set_so_home_path(string(TEST_DATA_PATH) + "/lib");
  plugin_info_ptr->plugin_conf.add_name("delivery");
  plugin_name = "delivery_test";
  int ret = plugin_manager_test_->LoadPlugin(plugin_info_ptr);
  EXPECT_NE(RET_OK, ret);
  plugin_name = "libdelivery_test.so";
  plugin_info_ptr->plugin_conf.set_so_name(plugin_name);
  ret = plugin_manager_test_->LoadPlugin(plugin_info_ptr);
  EXPECT_EQ(0, ret);
}

TEST_F(PluginManagerTest, LoadPluginsTest)
{
  int ret = plugin_manager_test_->LoadPlugins();
  EXPECT_EQ(RET_OK, ret);
}

TEST_F(PluginManagerTest, GetPluginTest)
{
  string query_name = "delivery";
  HandlerPlugin* plugin = static_cast<HandlerPlugin*>(plugin_manager_test_->GetPlugin(query_name));
  ASSERT_TRUE(plugin != NULL);

  query_name = "click";
  plugin =  static_cast<HandlerPlugin*>(plugin_manager_test_->GetPlugin(query_name));
  ASSERT_TRUE(plugin != NULL);
}

TEST_F(PluginManagerTest, ParseStr2MapTest)
{
  string content="ad==100";
  STR_MAP content_map;
  plugin_manager_test_->ParseStr2Map(content, content_map);
  EXPECT_STREQ("=100", content_map["ad"].c_str());
  content="ad=";
  plugin_manager_test_->ParseStr2Map(content, content_map);
  EXPECT_STREQ("", content_map["ad"].c_str());
}

