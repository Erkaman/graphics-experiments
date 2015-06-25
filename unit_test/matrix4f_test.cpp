#include "matrix4f_test.hpp"

#include "math/matrix4f.hpp"
#include "framework.hpp"

static void TestCreateTranslation() {
    Matrix4f trans = Matrix4f::CreateTranslation(1,2,3);
    Matrix4f expected(
	1,0,0,1,
	0,1,0,2,
	0,0,1,3,
	0,0,0,1
	);

    AssertEquals(trans, expected);
}

static void TestEquals() {
    Matrix4f a(
	1,2,3,4,
	5,6,7,8,
	9,10,11,12,
	13,14,15,16
	);

    Matrix4f b(
	1,2,3,4,
	5,6,7,8,
	9,10,11,12,
	13,14,15,16
	);

    AssertEquals(a,b);
}

static void TestMult() {
    Matrix4f a(
	1,2,3,4,
	5,6,7,8,
	9,10,11,12,
	13,14,15,16
	);

    Matrix4f b(
	17,18,19,20,
	21,22,23,24,
	25,26,27,28,
	29,30,31,32

	);

    Matrix4f expected(
	250,260,270,280,
	618,644,670,696,
	986,1028,1070,1112,
	1354,1412,1470,1528
	);

    AssertEquals(a*b, expected);
}

static void TestCreatePerspective() {
    Matrix4f expected(
	1.810660005f,0.0f       ,0.0f        ,0.0f,
	0.0f       ,2.414213657f,0.0f        ,0.0f,
	0.0f       ,0.0f       ,-1.002002001f,-0.200200200f,
	0.0f       ,0.0f       ,-1.0f       ,0.0f
	);

    Matrix4f pers = Matrix4f::CreatePerspective(45.0f, 400.0f/300.0f, 0.1f,100.0f);

    AssertEquals(pers, expected);
}
void Matrix4fTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestEquals, "TestEquals");
    suite.emplace_back(TestCreateTranslation, "TestCreateTranslation");
    suite.emplace_back(TestMult, "TestMult");
    suite.emplace_back(TestCreatePerspective, "TestCreatePerspective");

    RunSuite(suite, "Matrix4f");
}
