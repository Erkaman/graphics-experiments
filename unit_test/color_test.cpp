#include "color_test.hpp"
#include "framework.hpp"
#include "math/color.hpp"

static void TestLerp() {
    const Color a(0.0f, 0.5f, 0.3f);
    const Color b(0.5f, 1.0f, 0.6f);
    const float v = 0.5f;

    const Color expected(0.25f, 0.75f, 0.45f);


    AssertEquals(Color::Lerp(a,b,v), expected);
}

void ColorTestSuite() {
    TestSuite suite;

    suite.emplace_back(TestLerp, "TestLerp");

    RunSuite(suite, "Color");
}
