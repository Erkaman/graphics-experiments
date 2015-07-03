#include "vector4f_test.hpp"

#include "math/vector4f.hpp"

#include "framework.hpp"


static void TestAdd() {
    Vector4f a(1,2,3.5,2);
    Vector4f b(4,-1,9.5,3);

    Vector4f sum = a + b;
    Vector4f expectedSum(5,1,13,5);

    AssertEquals(sum,expectedSum);
}

static void TestSub() {
    Vector4f a(7,2,3,2);
    Vector4f b(-2,3,-3,1);

    AssertEquals(a-b,Vector4f(9,-1,6,1));
}

static void TestNegation() {
    Vector4f v(-1,2,-3.5,1);
    Vector4f expected(1,-2,3.5,-1);

    AssertEquals(-v,expected);
}

static void TestLength() {
    Vector4f v(-2, 5, 9,2);

    AssertEqualsFloat(v.Length(),10.67707);
}

static void TestNormalize() {
    Vector4f v(2.5,0,0,0);
    v.Normalize();

    AssertEquals(v,Vector4f(1,0,0,0));

    v = Vector4f(3,1,2,3);
    v.Normalize();
    AssertEquals(v,Vector4f(0.62554f,0.20851f,0.41702f,0.62554f));
}

static void TestEquals() {
    Vector4f a(1.0f,1.0f,1.0f,1.0f);
    Vector4f b(1.0f,1.0f,1.0f,1.0);
    Vector4f c(1.1f,1.0f,1.0f,1.0f);
    Vector4f d(1.0f,1.1f,1.0f,1.0f);
    Vector4f e(1.0f,1.0f,1.1f,1.0f);
    Vector4f f(1.0f,1.0f,1.0f,1.1f);

    AssertEquals(a,b);
    AssertNotEquals(a,c);
    AssertNotEquals(a,d);
    AssertNotEquals(a,e);
    AssertNotEquals(a,f);
}


static void TestScale() {
    Vector4f a(2,3,4,5);

    AssertEquals(3*a,Vector4f(6,9,12,15));

}

static void TestPlusEquals() {
    Vector4f a(2,3,4,1);

    a += Vector4f(4,5,6,2);

    AssertEquals(a,Vector4f(6,8,10,3));
}

static void TestConversion() {
    Vector4f a(2,3,4,1);

    Vector3f expected(2,3,4);

    AssertEquals(Vector3f(a),expected);
}

void Vector4fTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestAdd, "TestAdd");
    suite.emplace_back(TestEquals, "TestEquals");
    suite.emplace_back(TestNegation, "TestNegation");
    suite.emplace_back(TestSub, "TestSub");
    suite.emplace_back(TestLength, "TestLength");
    suite.emplace_back(TestNormalize, "TestNormalize");
    suite.emplace_back(TestScale, "TestScale");
    suite.emplace_back(TestPlusEquals, "TestPlusEquals");
    suite.emplace_back(TestConversion, "TestConversion");

    RunSuite(suite, "Vector4f");
}
