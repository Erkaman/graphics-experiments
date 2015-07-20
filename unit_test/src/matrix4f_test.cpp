#include "matrix4f_test.hpp"

#include "math/matrix4f.hpp"
#include "framework.hpp"
#include "math/vector3f.hpp"
#include "math/vector4f.hpp"
#include "common.hpp"

static void TestCreateTranslation() {
    Matrix4f trans = Matrix4f::CreateTranslation(1,2,3);
    Matrix4f expected(
	1,0,0,1,
	0,1,0,2,
	0,0,1,3,
	0,0,0,1
	);

    AssertEquals(trans, expected);

    // now test the Vector3f version.
    trans = Matrix4f::CreateTranslation(Vector3f(1,2,3));
    expected = Matrix4f(
	1,0,0,1,
	0,1,0,2,
	0,0,1,3,
	0,0,0,1
	);

    AssertEquals(trans, expected);
}

static void TestCreateScale() {
    Matrix4f trans = Matrix4f::CreateScale(2,3,4);
    Matrix4f expected(
	2,0,0,0,
	0,3,0,0,
	0,0,4,0,
	0,0,0,1
	);

    AssertEquals(trans, expected);

    // now test the Vector3f version.
    trans = Matrix4f::CreateScale(Vector3f(2,3,4));
    expected = Matrix4f(
	2,0,0,0,
	0,3,0,0,
	0,0,4,0,
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

static void TestTranspose() {
    Matrix4f a(
	1,2,3,4,
	5,6,7,8,
	9,10,11,12,
	13,14,15,16
	);

    Matrix4f expected(
	1,5,9,13,
	2,6,10,14,
	3,7,11,15,
	4,8,12,16
	);

    a.Transpose();
    AssertEquals(a, expected);
}


static void TestDeterminant() {
    Matrix4f a(
	2,5,3,5,
	4,6,6,3,
	11,3,2,-2,
	4,-7,9,3
	);

    AssertEqualsFloat(a.Determinant(), 2960);
}

static void TestTrace() {
    Matrix4f a(
	1,2,3,4,
	5,6,7,8,
	9,10,11,12,
	13,14,15,16
	);

    AssertEqualsFloat(a.Trace(), 34);
}

static void TestScale() {
    Matrix4f a(
	1,2,3,4,
	5,6,7,8,
	9,10,11,12,
	13,14,15,16
	);

    Matrix4f expected(
	5,10,15,20,
	25,30,35,40,
	45,50,55,60,
	65,70,75,80
	);

    AssertEquals(5*a, expected);
    AssertEquals(a*5, expected);

}

static void TestDiv() {
    Matrix4f expected(
	1,2,3,4,
	5,6,7,8,
	9,10,11,12,
	13,14,15,16
	);

    Matrix4f a(
	5,10,15,20,
	25,30,35,40,
	45,50,55,60,
	65,70,75,80
	);

    AssertEquals(a/5.0f, expected);
}


static void TestAdd() {

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
	18,20,22,24,
	26,28,30,32,
	34,36,38,40,
	42,44,46,48
	);

    AssertEquals(a+b, expected);
    AssertEquals(b+a, expected);

}

static void TestSub() {

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
	16,16,16,16,
	16,16,16,16,
	16,16,16,16,
	16,16,16,16
	);

    AssertEquals(b-a, expected);


}

static void TestInverse() {

    Matrix4f a(
	9,2,3,9,
	5,6,7,8,
	9,11,11,12,
	13,14,15,16
	);


    Matrix4f expected(
	8,-67,-40,59,
	0,-72,144,-72,
	-24,57,-168,111,
	16,64,64,-80
	);
    expected = expected / 144.0f;

    AssertEquals(a.Inverse(), expected);
}


static void MultVector() {

    Matrix4f a(
	1,2,3,4,
	5,6,7,8,
	9,10,11,12,
	13,14,15,16);

    Vector4f b(21,22,23,24);
    Vector4f expected(230,590,950,1310);

    AssertEquals(a*b, expected);
}

static void TestCreateRotate() {

    // rotate the vector (1,0,0,1) round the z-axis 90 degrees.
    AssertEquals(
	Matrix4f::CreateRotate(90, Vector3f(0,0,1) ) * Vector4f(1,0,0,1),
	Vector4f(0,1,0,1));

    AssertEquals(
	Matrix4f::CreateRotate(-90, Vector3f(0,0,1) ) * Vector4f(1,0,0,1),
	Vector4f(0,-1,0,1));

    AssertEquals(
	Matrix4f::CreateRotate(180, Vector3f(0,0,1) ) * Vector4f(1,0,0,1),
	Vector4f(-1,0,0,1));

    AssertEquals(
	Matrix4f::CreateRotate(90, Vector3f(0,1,0) ) * Vector4f(1,0,0,1),
	Vector4f(0,0,-1,1));

    AssertEquals(
	Matrix4f::CreateRotate(45, Vector3f(0,0,1) ) * Vector4f(1,0,0,1),
	Vector4f(0.707107,0.707107,0,1));


}

void Matrix4fTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestEquals, "TestEquals");
    suite.emplace_back(TestCreateTranslation, "TestCreateTranslation");
    suite.emplace_back(TestMult, "TestMult");
    suite.emplace_back(TestCreatePerspective, "TestCreatePerspective");
    suite.emplace_back(TestTranspose, "TestTranspose");
    suite.emplace_back(TestDeterminant, "TestDeterminant");
    suite.emplace_back(TestTrace, "TestTrace");
    suite.emplace_back(TestScale, "TestScale");
    suite.emplace_back(TestDiv, "TestDiv");
    suite.emplace_back(TestAdd, "TestAdd");
    suite.emplace_back(TestSub, "TestSub");
    suite.emplace_back(TestInverse, "TestInverse");
    suite.emplace_back(MultVector, "MultVector");
    suite.emplace_back(TestCreateScale, "TestCreateScale");
    suite.emplace_back(TestCreateRotate, "TestCreateRotate");



    RunSuite(suite, "Matrix4f");

}
