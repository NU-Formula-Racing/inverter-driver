#define NATIVE
#include "unity.h"

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void SampleTest(void)
{
    TEST_ASSERT_EQUAL_INT(true, true);
    TEST_ASSERT_NOT_EQUAL(true, false);
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(SampleTest);
    return UNITY_END();
}

int main(void) { return runUnityTests(); }
