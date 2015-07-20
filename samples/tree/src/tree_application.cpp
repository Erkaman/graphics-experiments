#include "tree_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"

#include "ewa/math/vector4f.hpp"

#include "tree.hpp"



using namespace std;

TreeApplication::TreeApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_tree(NULL){ }

TreeApplication::~TreeApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_tree);
}

void TreeApplication::Init() {

    ::SetDepthTest(true);
    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth(),GetWindowHeight(),Vector3f(1,0.5f,1), Vector3f(-1.0f,-0.5f,-1.0f), true);


    m_tree = new Tree(Vector3f(0,0,0));


}

void TreeApplication::Render() {

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f);

    const Vector4f lightPosition(93,10.0f,93, 1.0f);

    m_tree->Draw(*m_camera, lightPosition);
}

void TreeApplication::Update(const float delta) {
    m_camera->HandleInput(delta);
}

void TreeApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
