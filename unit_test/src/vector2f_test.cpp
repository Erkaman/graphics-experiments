#include "vector2f_test.hpp"

#include "math/vector2f.hpp"

#include "framework.hpp"


static void TestAdd() {
    Vector2f a(1.0f,2.2f);
    Vector2f b(4.0f,-1.1f);

    Vector2f sum = a + b;
    Vector2f expectedSum(5.0f,1.1f);

    AssertEquals(sum,expectedSum);
}

static void TestSub() {
    Vector2f a(7,2);
    Vector2f b(-2,2);

    AssertEquals(a-b,Vector2f(9,0));
}

static void TestNegation() {
    Vector2f v(1,-2.5);
    Vector2f expected(-1,2.5);

    AssertEquals(-v,expected);
}

static void TestLength() {
    Vector2f v(-2, 5);

    AssertEqualsFloat(v.Length(),5.38516f);
}

static void TestNormalize() {
    Vector2f v(2.5f,0.0f);
    v.Normalize();

    AssertEquals(v,Vector2f(1.0f,0.0f));

    v = Vector2f(2,3);
    v.Normalize();
    AssertEquals(v,Vector2f(.55470f, 0.83205f));
}

static void TestEquals() {
    Vector2f a(1.0f,1.0f);
    Vector2f b(1.0f,1.0f);
    Vector2f c(1.1f,1.0f);
    Vector2f d(1.0f,1.1f);

    AssertEquals(a,b);
    AssertNotEquals(a,c);
    AssertNotEquals(a,d);
}

static void TestScale() {
    Vector2f a(2,4);

    AssertEquals(2*a,Vector2f(4,8));
    AssertEquals(a*2,Vector2f(4,8));


    Vector2f b(2,4);
    b *= 2;
    AssertEquals(b,Vector2f(4,8));
}

static void TestPlusEquals() {
    Vector2f a(2,4);

    a += Vector2f(4,6);

    AssertEquals(a,Vector2f(6,10));
}

static void TestMinusEquals() {
    Vector2f a(2,3);

    a -= Vector2f(4,5);

    AssertEquals(a,Vector2f(-2,-2));
}

void Vector2fTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestAdd, "TestAdd");
    suite.emplace_back(TestEquals, "TestEquals");
    suite.emplace_back(TestNegation, "TestNegation");
    suite.emplace_back(TestSub, "TestSub");
    suite.emplace_back(TestLength, "TestLength");
    suite.emplace_back(TestNormalize, "TestNormalize");
    suite.emplace_back(TestScale, "TestScale");
    suite.emplace_back(TestPlusEquals, "TestPlusEquals");
    suite.emplace_back(TestMinusEquals, "TestMinusEquals");

    RunSuite(suite, "Vector2f");
}
