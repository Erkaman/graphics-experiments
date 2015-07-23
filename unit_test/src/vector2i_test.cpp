#include "vector2i_test.hpp"

#include "math/vector2i.hpp"

#include "framework.hpp"


static void TestAdd() {
    Vector2i a(1,3);
    Vector2i b(-1,2);

    AssertEquals(a+b, Vector2i(0,5));
}

static void TestSub() {
    Vector2i a(7,2);
    Vector2i b(-2,2);

    AssertEquals(a-b,Vector2i(9,0));
}

static void TestNegation() {
    Vector2i v(-1,2);
    Vector2i expected(1,-2);

    AssertEquals(-v,expected);
}

static void TestLength() {
    Vector2i v(-2, 5);

    AssertEqualsFloat(v.Length(),5.38516f);
}


static void TestEquals() {
    Vector2i a(1,1);
    Vector2i b(1,1);
    Vector2i c(2,1);
    Vector2i d(1,2);

    AssertEquals(a,b);
    AssertNotEquals(a,c);
    AssertNotEquals(a,d);
}

static void TestScale() {
    Vector2i a(2,4);

    AssertEquals(2*a,Vector2i(4,8));

    Vector2i b(2,4);
    b *= 2;
    AssertEquals(b,Vector2i(4,8));

}

static void TestPlusEquals() {
    Vector2i a(2,4);

    a += Vector2i(4,6);

    AssertEquals(a,Vector2i(6,10));
}

static void TestMinusEquals() {
    Vector2i a(2,4);

    a -= Vector2i(4,7);

    AssertEquals(a,Vector2i(-2,-3));
}


void Vector2iTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestAdd, "TestAdd");
    suite.emplace_back(TestEquals, "TestEquals");
    suite.emplace_back(TestNegation, "TestNegation");
    suite.emplace_back(TestSub, "TestSub");
    suite.emplace_back(TestLength, "TestLength");
    suite.emplace_back(TestScale, "TestScale");
    suite.emplace_back(TestPlusEquals, "TestPlusEquals");
    suite.emplace_back(TestMinusEquals, "TestMinusEquals");

    RunSuite(suite, "Vector2i");
}
