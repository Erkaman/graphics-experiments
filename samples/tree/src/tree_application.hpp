#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

class Camera;
class Tree;


class TreeApplication : public Application{

private:

    Camera* m_camera;
    Tree* m_tree;

public:

    TreeApplication(int argc, char *argv[]);
    ~TreeApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;
};
