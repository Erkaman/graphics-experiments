#include "perlin_seed.hpp"

#include "gl/shader_program.hpp"
#include "gl/texture2d.hpp"


int PerlinSeed::GetPermvalue(int i){
    return permutationArray[i % 256];
}

PerlinSeed::PerlinSeed() {

    GLfloat* data = (GLfloat*)malloc(sizeof(GLfloat) * 256 * 256 * 4);

    int i = 0;

	for(int y = 0; y < 256; y++){
	    for(int x = 0; x < 256; x++){
	    int A =  GetPermvalue(x) + y;
            int AA = GetPermvalue(A);
            int AB = GetPermvalue(A + 1);
            int B =  GetPermvalue(x + 1) + y;
            int BA = GetPermvalue(B);
            int BB = GetPermvalue(B + 1);



	    data[i++] = (float)AA/255.0f;
	    data[i++] = (float)AB/255.0f;
	    data[i++] = (float)BA/255.0f;
	    data[i++] = (float)BB/255.0f;

	}
    }

    permutationTable = std::make_unique<Texture2D>((GLvoid* )data, 256, 256, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    free(data);

    permutationTable->Bind();

    permutationTable->SetTextureRepeat();
    permutationTable->SetMinFilter(GL_LINEAR);
    permutationTable->SetMagFilter(GL_LINEAR);

    permutationTable->Unbind();


    data = (GLfloat*)malloc(sizeof(GLfloat) * 256 * 1 * 4);

    i = 0;

    for(int x = 0; x < 256; x++){
	int y = 0;

	data[i++] = grads[permutationArray[x] % 16][0];
	data[i++] = grads[permutationArray[x] % 16][1];
	data[i++] = grads[permutationArray[x] % 16][2];
	data[i++] = 1.0f;
    }

    gradTable = std::make_unique<Texture2D>((GLvoid* )data, 256, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    free(data);

    gradTable->Bind();

    /*
      fix tiling of s and t.
      also, make sure that texture is correctly bound.
     */

    gradTable->SetTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    gradTable->SetMinFilter(GL_LINEAR);
    gradTable->SetMagFilter(GL_LINEAR);

    gradTable->Unbind();
}

void PerlinSeed::Bind(ShaderProgram& shader) {
    shader.SetUniform("permSampler2d", 0);
    shader.SetUniform("permGradSampler", 1);


    Texture::SetActiveTextureUnit(0);
    permutationTable->Bind();

    Texture::SetActiveTextureUnit(1);
    gradTable->Bind();
}

void PerlinSeed::Unbind() {
    permutationTable->Unbind();
    gradTable->Unbind();
}
