#include <gtest/gtest.h>
#include <monitor/common/log.h>
#include <monitor/test/test.h>

using namespace std;
class shareddistEnvironment : public testing::Environment
{
public:
    virtual void SetUp()
    {
        LOG_CONFIG(DOTEST_LOGGER_CONF);
    }
    virtual void TearDown()
    {
        std::cout << "Foo FooEnvironment TearDown" << std::endl;
    }
};

int main(int argc, char** argv) {
    testing::AddGlobalTestEnvironment(new shareddistEnvironment);
    testing::InitGoogleTest(&argc, argv);
    // Runs all tests using Google Test.
    int ret = RUN_ALL_TESTS();
    LOG_SHUTDOWN();
    return ret;
}
