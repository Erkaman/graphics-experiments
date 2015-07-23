#include "vector3i_test.hpp"

#include "math/vector3i.hpp"

#include "framework.hpp"


static void TestAdd() {
    Vector3i a(1,2,3);
    Vector3i b(-1,2,4);

    AssertEquals(a+b, Vector3i(0,4,7));
}

static void TestSub() {
    Vector3i a(7,2,3);
    Vector3i b(-2,3,-3);

    AssertEquals(a-b,Vector3i(9,-1,6));
}

static void TestNegation() {
    Vector3i v(-1,2,-3);
    Vector3i expected(1,-2,3);

    AssertEquals(-v,expected);
}

static void TestLength() {
    Vector3i v(-2, 5, 9);

    AssertEqualsFloat(v.Length(),10.48808f);
}


static void TestEquals() {
    Vector3i a(1,1,1);
    Vector3i b(1,1,1);
    Vector3i c(2,1,1);
    Vector3i d(1,2,1);
    Vector3i e(1,1,2);

    AssertEquals(a,b);
    AssertNotEquals(a,c);
    AssertNotEquals(a,d);
    AssertNotEquals(a,e);
}

static void TestCross() {
    Vector3i a(2,3,4);
    Vector3i b(5,6,7);

    AssertEquals(Vector3i::Cross(a,b),Vector3i(-3,6,-3));
}

static void TestScale() {
    Vector3i a(2,3,4);

    AssertEquals(3*a,Vector3i(6,9,12));

    Vector3i b(2,3,4);
    b *= 3;
    AssertEquals(b,Vector3i(6,9,12));

}

static void TestPlusEquals() {
    Vector3i a(2,3,4);

    a += Vector3i(4,5,6);

    AssertEquals(a,Vector3i(6,8,10));
}

static void TestMinusEquals() {
    Vector3i a(2,3,4);

    a -= Vector3i(4,5,7);

    AssertEquals(a,Vector3i(-2,-2,-3));
}


void Vector3iTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestAdd, "TestAdd");
    suite.emplace_back(TestEquals, "TestEquals");
    suite.emplace_back(TestNegation, "TestNegation");
    suite.emplace_back(TestSub, "TestSub");
    suite.emplace_back(TestLength, "TestLength");
    suite.emplace_back(TestCross, "TestCross");
    suite.emplace_back(TestScale, "TestScale");
    suite.emplace_back(TestPlusEquals, "TestPlusEquals");
    suite.emplace_back(TestMinusEquals, "TestMinusEquals");

    RunSuite(suite, "Vector3i");
}
