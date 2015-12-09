#include "tuhu_application.hpp"

#include <iostream>

#include <btBulletDynamicsCommon.h>

int main (int argc, char *argv[]) {


    TuhuApplication app(argc, argv);
    app.Start();
    return 0;
}
