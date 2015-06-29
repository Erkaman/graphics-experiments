#include "mult_array_test.hpp"

#include "mult_array.hpp"

#include "framework.hpp"

typedef MultArray<int> IMultArray;

static void TestGet() {

    IMultArray m1(2,2);
    printf("wat\n");
    m1.Get(0,0) = 1;
    m1.Get(1,0) = 2;
    m1.Get(0,1) = 3;
    m1.Get(1,1) = 4;

    IMultArray m2(m1);

    AssertEquals(m1,m2);

    m1.Get(0,0) = 9;

    AssertNotEquals(m1,m2);

    m2.Get(0,0) = 9;

    AssertEquals(m1,m2);
}

static void TestGetters() {

    IMultArray m1(4,7);

    AssertEquals(4,m1.GetXsize());
    AssertEquals(7,m1.GetYsize());
    AssertEquals(28,m1.GetTotalsize());
}

static void TestAssignment() {
    IMultArray m1(1,2);

    m1.Get(0,0) = 2;
    m1.Get(0,1) = 3;

    IMultArray m2(m1);

    m2.Get(0,0) = 3;

    m2 = m1;

    AssertEquals(m1,m2);


}

void MultArrayTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestGet, "TestGet");
    suite.emplace_back(TestGetters, "TestGetters");
    suite.emplace_back(TestAssignment, "TestAssignment");

    RunSuite(suite, "MultArray");
}
