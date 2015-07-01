#include "mult_array_test.hpp"

#include "mult_array.hpp"

#include "framework.hpp"

typedef MultArray<int> IMultArray;
typedef MultArray<int>::iterator iterator;

static void TestGet() {

    IMultArray m1(2,2);
    m1(0,0) = 1;
    m1(1,0) = 2;
    m1(0,1) = 3;
    m1(1,1) = 4;

    IMultArray m2(m1);

    AssertEquals(m1,m2);

    m1(0,0) = 9;

    AssertNotEquals(m1,m2);

    m2(0,0) = 9;

    AssertEquals(m1,m2);
}

static void TestGetters() {

    IMultArray m1(4,7);

    AssertEqualsInt(4,m1.GetXsize());
    AssertEqualsInt(7,m1.GetYsize());
    AssertEqualsInt(28,m1.GetTotalsize());
}

static void TestAssignment() {
    IMultArray m1(1,2);

    m1(0,0) = 2;
    m1(0,1) = 3;

    IMultArray m2(m1);

    m2(0,0) = 3;

    m2 = m1;

    AssertEquals(m1,m2);
}

static void TestIterator() {
    IMultArray m1(2,2);
    m1(0,0) = 1;
    m1(1,0) = 2;
    m1(0,1) = 3;
    m1(1,1) = 4;

    iterator it = m1.Begin();


    AssertEqualsInt(*it,1);

    ++it;

    AssertEqualsInt(*it,2);

    ++it;

    AssertEqualsInt(*it,3);

    ++it;

    AssertEqualsInt(*it,4);
}

static void TestGetWrap() {

    IMultArray m1(2,3);
    m1(0,0) = 1;
    m1(1,0) = 2;
    m1(0,1) = 3;
    m1(1,1) = 4;
    m1(0,2) = 5;
    m1(1,2) = 6;

    AssertEqualsInt(m1.GetWrap(0,3),5);
    AssertEqualsInt(m1.GetWrap(1,3),6);


    AssertEqualsInt(m1.GetWrap(0,-1),1);
    AssertEqualsInt(m1.GetWrap(2,-1),2);

    AssertEqualsInt(m1.GetWrap(2,1),4);
    AssertEqualsInt(m1.GetWrap(2,2),6);
}

void MultArrayTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestGet, "TestGet");
    suite.emplace_back(TestGetters, "TestGetters");
    suite.emplace_back(TestAssignment, "TestAssignment");
    suite.emplace_back(TestIterator, "TestIterator");
    suite.emplace_back(TestGetWrap, "TestGetWrap");

    RunSuite(suite, "MultArray");
}
