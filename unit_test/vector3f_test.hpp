#pragma once

#include "math/vector3f.hpp"

#include "framework.hpp"

void TestAdd() {
    BeginMethod("TestAdd");

    Vector3f a(1,2,3.5);
    Vector3f b(4,-1,9.5);

    Vector3f sum = a + b;
    Vector3f expectedSum(5,1,13);

    AssertEquals(sum,expectedSum);

    EndMethod("TestAdd");
}

void TestEquals() {
    BeginMethod("TestEquals");

    Vector3f a(1.0f,1.0f,1.0f);
    Vector3f b(1.0f,1.0f,1.0f);
    Vector3f c(1.1f,1.0f,1.0f);
    Vector3f d(1.0f,1.1f,1.0f);
    Vector3f e(1.0f,1.0f,1.1f);

    AssertEquals(a,b);
    AssertNotEquals(a,c);
    AssertNotEquals(a,d);
    AssertNotEquals(a,e);

    EndMethod("TestEquals");
}

void Vector3fTestSuite() {
    BeginSuite("Vector3f");
    TestAdd();
    TestEquals();
    EndSuite("Vector3f");
}
