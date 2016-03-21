#include <gtest/gtest.h>
#include "../time_utility.h"
#include <sharelib/test/test.h>
using namespace std;

using namespace sharelib;

TEST(TimeUtility, readable)
{
    cout << TimeUtility::CurrentTimeDateReadable()<< endl;
    cout << TimeUtility::CurrentTimeInSecondsReadable()<< endl;
    cout << TimeUtility::CurrentTimeInSeconds() - (TimeUtility::CurrentTimeInSeconds() /86400) * 86400<< endl;
    cout << TimeUtility::CurrentTimeInSeconds() << endl;
    cout << TimeUtility::GetSecondsOfDayBegin() << endl;
    cout << TimeUtility::GetSecondFromString("2012-08-09 12:13:22") << endl;
}

