#include "height_map.hpp"

#include <vector>
#include "lodepng.h"

#include "log.hpp"

#include "gl/vbo.hpp"

using std::unique_ptr;
using std::vector;

HeightMap::HeightMap(const std::string& path) {


    /*
      Load the heightmap data.
     */

    std::vector<unsigned char> buffer;
    lodepng::load_file(buffer, path);

    lodepng::State state;
    std::vector<unsigned char> imageData;
    unsigned error = lodepng::decode(imageData, m_width, m_depth, state, buffer);

    if(error != 0){
	LOG_E("could not load height map: %s", lodepng_error_text(error));
    }

    /*
      Next we create the vertex buffer.
     */

    int xpos = 0;
    int zpos = 0;

    vertexBuffer = unique_ptr<VBO>(VBO::CreatePosition(3));

    FloatVector vertices;

    for(size_t i = 0; i < imageData.size(); i+=4) {

	if(i != 0 && ( (i) % (4 * m_width) == 0)) {
	    ++zpos;
	    xpos = 0;
	}

	LOG_I("%ld: %d, %d", i/4, xpos, zpos);

	float ypos = ComputeY(imageData[i]);

	vertices.push_back(xpos);
	vertices.push_back(ypos);
	vertices.push_back(zpos);

	++xpos;

    }

    vertexBuffer->Bind();
    vertexBuffer->SetBufferData(vertices);
    vertexBuffer->Unbind();

    /*
      Finally, we create the index buffer.
     */


    indexBuffer = unique_ptr<VBO>(VBO::CreateIndex());

    UshortVector indices;

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(4);

    indexBuffer->Bind();
    indexBuffer->SetBufferData(indices);
    indexBuffer->Unbind();
    /*for(int x = 0; x < (m_width-1); ++x) {

	for(int z = 0; z < (m_depth-1); ++x) {


	}
	}*/

}

void HeightMap::Draw() {

    vertexBuffer->Bind();
    vertexBuffer->EnableVertexAttrib();
    vertexBuffer->Bind();

    indexBuffer->Bind();

    indexBuffer->DrawIndices(GL_TRIANGLES, 3);


    indexBuffer->Unbind();

    vertexBuffer->Bind();
    vertexBuffer->DisableVertexAttrib();
    vertexBuffer->Bind();

}


const float HeightMap::ComputeY(const unsigned char heightMapData ) {
    return (float)heightMapData  / 255.0f;
}
