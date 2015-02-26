#include "tuhu_application.h"

#include "file.h"

void TuhuApplication::Init() {

    LOG_I("file content:" + File::GetFileContents("hello"));


    GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing
}

void TuhuApplication::Render() {

    SetViewport();
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void TuhuApplication::Update() {
}
