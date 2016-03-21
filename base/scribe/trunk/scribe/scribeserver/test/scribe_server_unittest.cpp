#include <gtest/gtest.h>
#include <string>
#include <scribe/test/test.h> 
#include <sharelib/util/string_util.h> 
#include <sharelib/util/file_util.h> 
#include "../scribe_server.h"
#include "fake_scribe_log_handler.h"
using namespace scribe_server;
using namespace std;
using namespace sharelib;
#define __UTEST__
class ScribeServerTest:public testing::Test {
  protected:
    static void SetUpTestCase(){
      scribe_server_test_.reset(new ScribeServer());
      ScribeLogHandlerPtr fake_scribe_log_ptr(new FakeScribeLogHandler());
      scribe_server_test_->SetScribeLogHandler(fake_scribe_log_ptr);
    }
    static void TearDownTestCase() {
    }
    static ScribeServerPtr scribe_server_test_;
};

ScribeServerPtr ScribeServerTest::scribe_server_test_;

TEST_F(ScribeServerTest, InitTest) {
  string oldStr = "TO_REPLACE/";                                                                       
  string server_config = string(TEST_DATA_PATH) + "/conf/scribeserver.conf";

  string scribe_client_tool_file = string(TEST_DATA_PATH) + "/conf/scribe_client_tool.sh-test";

  string config_file = string(string(TEST_DATA_PATH) + "/conf/scribe_client_tool.sh");

  string cp_file_cmd = "cp "+  scribe_client_tool_file + " " + config_file;
  int system_cmd = system(cp_file_cmd.c_str());
  string content = FileUtil::ReadLocalFileOriginalData(config_file);                                   
  StringUtil::ReplaceAll(content, oldStr, string(TEST_DATA_PATH));                                     
  FileUtil::WriteLocalFile(config_file, content);                 
  int ret = scribe_server_test_->Init(server_config.c_str());
  ScribeClientForUnitTest scribe_client_for_unit;
  LogEntry log_entry;
  log_entry.__set_message("message test1");
  log_entry.__set_category("category test1");
  scribe_client_for_unit.Run();
 // scribe_server_test_->Start();
  EXPECT_EQ(0, ret);
  sleep(10);
  scribe_server_test_->Stop();
}

