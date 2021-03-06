#pragma once

#include "ewa/geometry_object.hpp"

#include <vector>

class VBO;
class ShaderProgram;
class ICamera;
class Texture;
class CloudGroup;
class Random;

class Skydome /*: public GeometryObject*/{

private:

    /*
      There variables are used to draw the dome:
     */

    unsigned int m_domeNumTriangles;
    VBO* m_domeVertexBuffer;
    VBO* m_domeIndexBuffer;
    ShaderProgram* m_domeShader;

    /*
      These are used to draw the billboard sun:
     */
    VBO* m_sunVertexBuffer;
    VBO* m_sunIndexBuffer;

    Texture* m_sunTexture;



    /*
      Used for drawing billboards:
     */

    ShaderProgram* m_billboardShader;


    std::vector<CloudGroup*> m_clouds;

    Random* m_rng;



    /*
      PRIVATE INSTANCE METHODS
     */
    void DrawDome(const ICamera* camera);
    void DrawSun(const ICamera* camera);
    void DrawClouds(const ICamera* camera);


    /*
      orientation, elevation and rotation are all angles measured in degrees.
     */
    void DrawBillboard(const ICamera* camera, VBO* m_vertexBuffer, VBO* m_indexBuffer,
		       const float orientation, const float elevation, const float rotation);

    void MakeSky(const float radius, const int slices, const int stacks);
    void MakeSun();
    void MakeClouds();


public:

    Skydome(const float radius, const int slices, const int stacks);
    ~Skydome();

    void Draw(const ICamera* camera);
    void Update(const float delta);


};
