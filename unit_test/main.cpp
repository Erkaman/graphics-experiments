#include "vector3f_test.hpp"
#include "matrix4f_test.hpp"
#include "log.hpp"
#include <stdio.h>

int main() {

    LogInit();

    Vector3fTestSuite();
    Matrix4fTestSuite();

    printf("ALL TESTS PASSED!");

    LogDispose();

}
