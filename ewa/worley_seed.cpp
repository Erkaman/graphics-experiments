#include "worley_seed.hpp"

#include "gl/shader_program.hpp"
#include "gl/texture2d.hpp"

#include "common.hpp"

#include "util.hpp"

/*int WorleySeed::PermValue(const int i){
    return perm[i % 256];
}

WorleySeed::~WorleySeed() {
    MY_DELETE(permTable);
    MY_DELETE(vectorTable);
}


WorleySeed::WorleySeed(unsigned long long seed):
    perm { 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180},
grad {{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1},{1,1,0},{0,-1,1},{-1,1,0},{0,-1,-1}} {

    Random rng(seed);
    Shuffle(rng, perm, perm+256);


    GLfloat* data = (GLfloat*)malloc(sizeof(GLfloat) * 256 * 256 * 4);

    int i = 0;

	for(int y = 0; y < 256; y++){
	    for(int x = 0; x < 256; x++){
	    int A =  PermValue(x) + y;
            int AA = PermValue(A);
            int AB = PermValue(A + 1);
            int B =  PermValue(x + 1) + y;
            int BA = PermValue(B);
            int BB = PermValue(B + 1);

	    data[i++] = (float)AA/255.0f;
	    data[i++] = (float)AB/255.0f;
	    data[i++] = (float)BA/255.0f;
	    data[i++] = (float)BB/255.0f;

	}
    }

    permTable = new Texture2D((GLvoid* )data, 256, 256, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    free(data);

    permTable->Bind();

    permTable->SetTextureRepeat();
    permTable->SetMinFilter(GL_LINEAR);
    permTable->SetMagFilter(GL_LINEAR);

    permTable->Unbind();


    data = (GLfloat*)malloc(sizeof(GLfloat) * 256 * 1 * 4);

    i = 0;

    for(int x = 0; x < 256; x++){
	int y = 0;

	data[i++] = grad[perm[x] % 16][0];
	data[i++] = grad[perm[x] % 16][1];
	data[i++] = grad[perm[x] % 16][2];
	data[i++] = 1.0f;
    }

    gradTable = new Texture2D((GLvoid* )data, 256, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    free(data);

    gradTable->Bind();


    gradTable->SetTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    gradTable->SetMinFilter(GL_LINEAR);
    gradTable->SetMagFilter(GL_LINEAR);

    gradTable->Unbind();
}

void WorleySeed::Bind(ShaderProgram& shader) {
    shader.SetUniform("permSampler2d", 0);
    shader.SetUniform("permGradSampler", 1);


    Texture::SetActiveTextureUnit(0);
    permTable->Bind();

    Texture::SetActiveTextureUnit(1);
    gradTable->Bind();
}

void WorleySeed::Unbind() {
    permTable->Unbind();
    gradTable->Unbind();
}
*/
