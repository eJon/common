#include <gtest/gtest.h>
#include <session/test/test.h>
#include <session/common/log.h>
using namespace std;
class shareddistEnvironment : public testing::Environment
{
public:
    virtual void SetUp()
    {
        cout << "file is " << DOTEST_LOGGER_CONF ;

    }
    virtual void TearDown()
    {
        
    }
};

int main(int argc, char** argv) {
    
    LOG_CONFIG(DOTEST_LOGGER_CONF);    
    testing::AddGlobalTestEnvironment(new shareddistEnvironment);
    testing::InitGoogleTest(&argc, argv);
    // Runs all tests using Google Test.
    
    int ret = RUN_ALL_TESTS();
    LOG_SHUTDOWN();
    return ret;
}
