#include <gtest/gtest.h>
#include "../ngx_handler.h"
#include "../ngx_adhandler_conf.h"
using namespace std;
using namespace ngx_handler;

enum {
  RET_OK = 0
};

class HandlerTest:public testing::Test
{
  protected:
    static void SetUpTestCase()
    {
      ngx_handler_test_ = new Handler();
 //     srandom(time(NULL));
    }
    static void TearDownTestCase()
    {
      delete ngx_handler_test_;
    }
    static Handler* ngx_handler_test_;
};
Handler* HandlerTest::ngx_handler_test_= NULL;

TEST_F(HandlerTest, InitTest)
{
  STR_MAP config_map;
  int ret = ngx_handler_test_->Init(config_map);
  EXPECT_EQ(RET_OK, ret);

}

TEST_F(HandlerTest, InitProcessTest)
{ 
  STR_MAP config_map;
  int ret = ngx_handler_test_->Init(config_map);
  EXPECT_EQ(RET_OK, ret);
  config_map[PLUGIN_MANAGER_CONF_FILE] = "/data0/kefeng2/work/20130410_wireless_tips_dev/sharelib/testdata/conf/plugin_manager.conf";
  ret = ngx_handler_test_->InitProcess();
  EXPECT_EQ(RET_OK, ret);
}

TEST_F(HandlerTest, HandleTest)
{
  STR_MAP query_map;
  string result;
  int ret = ngx_handler_test_->Handle(query_map, result);

}

