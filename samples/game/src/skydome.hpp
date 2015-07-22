#pragma once

#include "ewa/geometry_object.hpp"

class VBO;
class ShaderProgram;
class Camera;
class Texture;

class Skydome : public GeometryObject{

private:

    /*
      There variables are used to draw the dome:
     */

    unsigned int m_domeNumTriangles;
    VBO* m_domeVertexBuffer;
    VBO* m_domeIndexBuffer;
    ShaderProgram* m_domeShader;

    float m_delta;

    /*
      These are used to draw the billboard sun:
     */
    VBO* m_sunVertexBuffer;
    VBO* m_sunIndexBuffer;

    Texture* m_sunTexture;


    ShaderProgram* m_billboardShader;


    /*
      PRIVATE INSTANCE METHODS
     */
    void DrawDome(const Camera& camera);
    void DrawSun(const Camera& camera);


    /*
      orientation, elevation and rotation are all angles measured in degrees.
     */
    void DrawBillboard(const Camera& camera, VBO* m_vertexBuffer, VBO* m_indexBuffer,
		       const float orientation, const float elevation, const float rotation);

public:

    Skydome(const float radius, const int slices, const int stacks);
    ~Skydome();

    void Draw(const Camera& camera);
    void Update(const float delta);


    void MakeSky(const float radius, const int slices, const int stacks);

    void MakeSun();
};
