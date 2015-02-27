#include "tuhu_application.h"
#include "shader/shader_program.h"

#include "fileutil.h"

using namespace std;

void TuhuApplication::Init() {

/*    string s = GetFileContents("hello");

    LOG_I("LEN:" + tos(s.length()));
    LOG_I("size:" + tos(s.length()));

    LOG_I("file content:" + s);
    exit(1);
*/



    ShaderProgram shader("shader/font_render");

//    UniformLocationStore uls(0);


    GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing
}

void TuhuApplication::Render() {

    SetViewport();
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void TuhuApplication::Update() {
}
