#include "grid.hpp"

#include "ewa/camera.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/math/matrix4f.hpp"

using std::vector;

    // create the vertices of a unit cube.

    constexpr int GRID = 40;
    constexpr float GRID_SIZE = 2.0f/GRID;


static void  AddFace(
    FloatVector& positions,
    const float ax, const float ay,
    const float dx, const float dy
    ) {
    positions.push_back(ax);
    positions.push_back(ay);

    positions.push_back(ax+dx);
    positions.push_back(ay);

    positions.push_back(ax+dx);
    positions.push_back(ay+dy);



    positions.push_back(ax+dx);
    positions.push_back(ay+dy);

    positions.push_back(ax);
    positions.push_back(ay+dy);

    positions.push_back(ax);
    positions.push_back(ay);
}

Grid::Grid() {
    // load shader.
    m_shader = ShaderProgram::Load("shader/grid");


//    m_gridTexture= Texture2D::Load("img/body_ao.png");
    m_gridTexture= Texture2D::Load("img/Lenna.png");
//    m_gridTexture= Texture2D::Load("img/test_cloud2.png");

    if(!m_gridTexture) {
	PrintErrorExit();
    }

    m_gridTexture->Bind();
    m_gridTexture->SetTextureClamping();
    m_gridTexture->SetMinFilter(GL_LINEAR);
    m_gridTexture->SetMagFilter(GL_NEAREST);
    m_gridTexture->Unbind();

    // create the vertex data.

    FloatVector positions;


    for(int x = 0; x < GRID; ++x) {

	for(int y = 0; y < GRID; ++y) {

	    AddFace(positions,
		    -1 + x * GRID_SIZE,
		    -1 + y * GRID_SIZE,

		    GRID_SIZE,
		    GRID_SIZE
		);


	}

    }



/*
    AddFace(positions,
	    0,0,
	    +1,+1
	);

    AddFace(positions,
	    0,0,
	    +1,+1
	);
*/

    m_numTris = positions.size() / 6;

    m_positionBuffer = VBO::CreateInterleaved(
	    vector<GLuint>{2} // pos,
	    );

    m_positionBuffer->Bind();
    m_positionBuffer->SetBufferData(positions);
    m_positionBuffer->Unbind();
}

void Grid::Draw() {

    m_shader->Bind();

    m_shader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);
    m_gridTexture->Bind();

    m_shader->SetUniform("gridSize", GRID_SIZE);



    m_positionBuffer->EnableVertexAttribInterleavedWithBind();

//    m_indexBuffer->DrawIndices(GL_TRIANGLES, m_numIndices);
    // draw vertices.
    m_positionBuffer->DrawVertices(GL_TRIANGLES, m_numTris * 3);

    m_positionBuffer->DisableVertexAttribInterleavedWithBind();


    m_gridTexture->Unbind();

    m_shader->Unbind();
}
