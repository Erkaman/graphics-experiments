#include "vector3f_test.hpp"
#include "vector3i_test.hpp"
#include "vector2f_test.hpp"
#include "vector4f_test.hpp"
#include "matrix4f_test.hpp"
#include "color_test.hpp"
#include "mult_array_test.hpp"
#include "str_test.hpp"
#include "random_test.hpp"
#include "log.hpp"

int main() {

    LogInit();

    Vector2fTestSuite();
    Vector3fTestSuite();
    Vector3iTestSuite();
    Vector4fTestSuite();
    Matrix4fTestSuite();
    ColorTestSuite();
    MultArrayTestSuite();
    StrTestSuite();
    RandomTestSuite();

    printf("ALL TESTS PASSED!");

    LogDispose();
}
