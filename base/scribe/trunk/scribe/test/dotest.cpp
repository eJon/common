#include <gtest/gtest.h>
#include "scribe/common/scribe_debug_log.h"
#include <scribe/test/test.h>
#define _BIDFEED_UNIT_TEST_
using namespace std;
class shareddistEnvironment : public testing::Environment
{
public:
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
        std::cout << "Foo FooEnvironment TearDown" << std::endl;
    }
};

int main(int argc, char** argv) {
  LOG_CONFIG(DOTEST_LOGGER_CONF);    
  //BIDFEEDCOMMON_LOG_SET_LOGGER();
    testing::AddGlobalTestEnvironment(new shareddistEnvironment);
    testing::InitGoogleTest(&argc, argv);
    // Runs all tests using Google Test.
    return RUN_ALL_TESTS();
}

/*
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <matchserver/test/test.h>
#include <matchserver/config.h>
#include <matchserver/util/log.h>
#include <string>

using namespace std;
using namespace CppUnit;

int main( int argc, char **argv)
{
    MATCHSERVER_LOG_CONFIG(DOTEST_LOGGER_CONF);
    TextUi::TestRunner runner;
    runner.setOutputter(new CompilerOutputter(&runner.result(), std::cerr));
    TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    bool ok = runner.run("", false);
    
    MATCHSERVER_LOG_SHUTDOWN();
    return ok ? 0 : 1;
}
*/
