#include "vector3f_test.hpp"
#include "matrix4f_test.hpp"

#include <stdio.h>

int main() {
    Vector3fTestSuite();
    Matrix4fTestSuite();

    printf("ALL TESTS PASSED!");
}
