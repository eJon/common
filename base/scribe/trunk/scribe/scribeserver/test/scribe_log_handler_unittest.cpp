#include <gtest/gtest.h>
#include <string>
#include <scribe/test/test.h>
#include "fake_scribe_log_handler.h"
#include "../scribe_log_handler.h"
using namespace scribe_server;
using namespace std;
using namespace sharelib;
#define __UTEST__
class ScribeLogHandlerTest:public testing::Test {
  protected:
    static void SetUpTestCase(){
      scribe_log_test_.reset(new FakeScribeLogHandler());
    }
    static void TearDownTestCase() {
    }
    static ScribeLogHandlerPtr scribe_log_test_;
};

ScribeLogHandlerPtr ScribeLogHandlerTest::scribe_log_test_;
TEST_F(ScribeLogHandlerTest, InitTest) {
  string config_file = string(string(TEST_DATA_PATH) + "/conf/scribeserver.conf");
  CINIConfig ini_config(config_file.c_str());
  int ret = scribe_log_test_->Init(ini_config);
  scribe_log_test_->Start();
  vector<LogEntry> log_entries;
  LogEntry log_entry;
 /* int size =100;
  for (int i = 0; i < size; ++i) {
    stringstream ss;
    ss << "message test" << i;
    log_entry.__set_message(ss.str());
    log_entry.__set_category("category test");
    log_entries.push_back(log_entry);
  }
  scribe_log_test_->Log(log_entries);*/
  EXPECT_EQ(0, ret);
  sleep(10);
  scribe_log_test_->Stop();
}

