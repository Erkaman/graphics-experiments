#include "vector3f_test.hpp"
#include "vector4f_test.hpp"
#include "matrix4f_test.hpp"
#include "color_test.hpp"
#include "mult_array_test.hpp"
#include "str_test.hpp"
#include "random_test.hpp"
#include "log.hpp"
#include <stdio.h>


int main() {

    LogInit();

    Vector3fTestSuite();
    Vector4fTestSuite();
    Matrix4fTestSuite();
    ColorTestSuite();
    MultArrayTestSuite();
    StrTestSuite();
    RandomTestSuite();

    printf("ALL TESTS PASSED!");

    LogDispose();



}
