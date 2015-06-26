#include "vector3f_test.hpp"

#include "math/vector3f.hpp"

#include "framework.hpp"


static void TestAdd() {
    Vector3f a(1,2,3.5);
    Vector3f b(4,-1,9.5);

    Vector3f sum = a + b;
    Vector3f expectedSum(5,1,13);

    AssertEquals(sum,expectedSum);
}

static void TestSub() {
    Vector3f a(7,2,3);
    Vector3f b(-2,3,-3);

    AssertEquals(a-b,Vector3f(9,-1,6));
}

static void TestNegation() {
    Vector3f v(-1,2,-3.5);
    Vector3f expected(1,-2,3.5);

    AssertEquals(-v,expected);
}

static void TestLength() {
    Vector3f v(-2, 5, 9);

    AssertEquals(v.Length(),10.48808);
}

static void TestNormalize() {
    Vector3f v(2.5,0,0);
    v.Normalize();

    AssertEquals(v,Vector3f(1,0,0));

    v = Vector3f(3,1,2);
    v.Normalize();
    AssertEquals(v,Vector3f(.80178,.26726,.53452));
}

static void TestEquals() {
    Vector3f a(1.0f,1.0f,1.0f);
    Vector3f b(1.0f,1.0f,1.0f);
    Vector3f c(1.1f,1.0f,1.0f);
    Vector3f d(1.0f,1.1f,1.0f);
    Vector3f e(1.0f,1.0f,1.1f);

    AssertEquals(a,b);
    AssertNotEquals(a,c);
    AssertNotEquals(a,d);
    AssertNotEquals(a,e);
}

static void TestCross() {
    Vector3f a(2,3,4);
    Vector3f b(5,6,7);

    AssertEquals(Vector3f::Cross(a,b),Vector3f(-3,6,-3));
}

static void TestScale() {
    Vector3f a(2,3,4);

    AssertEquals(3*a,Vector3f(6,9,12));
}

static void TestPlusEquals() {
    Vector3f a(2,3,4);

    a += Vector3f(4,5,6);

    AssertEquals(a,Vector3f(6,8,10));
}

void Vector3fTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestAdd, "TestAdd");
    suite.emplace_back(TestEquals, "TestEquals");
    suite.emplace_back(TestNegation, "TestNegation");
    suite.emplace_back(TestSub, "TestSub");
    suite.emplace_back(TestLength, "TestLength");
    suite.emplace_back(TestNormalize, "TestNormalize");
    suite.emplace_back(TestCross, "TestCross");
    suite.emplace_back(TestScale, "TestScale");
    suite.emplace_back(TestPlusEquals, "TestPlusEquals");

    RunSuite(suite, "Vector3f");
}
