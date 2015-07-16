#include "value_noise_seed.hpp"

#include "perlin_seed.hpp"

#include "gl/shader_program.hpp"
#include "gl/texture2d.hpp"

#include "util.hpp"

int ValueNoiseSeed::PermValue(const int i){
    return perm[i % 256];
}

ValueNoiseSeed::~ValueNoiseSeed() {
    delete permTable;
    delete randomTable;
}


ValueNoiseSeed::ValueNoiseSeed(unsigned long long seed):
    perm { 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180},
rand {
    0.2243f, 0.88377f, 0.15254f, 0.11117f, 0.62566f, 0.44021f, 0.13131f, 0.32139f, 0.39726f, 0.23665f, 0.71921f, 0.22257f, 0.36372f, 0.1144f, 0.47209f, 0.42407f, 0.60414f, 0.07001f, 0.36664f, 0.81342f, 0.61698f, 0.29855f, 0.99254f, 0.97719f, 0.31562f, 0.38425f, 0.79983f, 0.88127f, 0.18637f, 0.92597f, 0.19957f, 0.2771f, 0.86967f, 0.2086f, 0.28202f, 0.28879f, 0.20218f, 0.33036f, 0.28863f, 0.85115f, 0.92013f, 0.194f, 0.3807f, 0.84181f, 0.12263f, 0.56493f, 0.02348f, 0.36991f, 0.68042f, 0.92319f, 0.88533f, 0.02064f, 0.33633f, 0.54189f, 0.60796f, 0.02078f, 0.59364f, 0.79311f, 0.54603f, 0.10079f, 0.97798f, 0.96615f, 0.50462f, 0.01244f, 0.66895f, 0.9966f, 0.9274f, 0.39116f, 0.40843f, 0.57159f, 0.90086f, 0.58121f, 0.2496f, 0.92347f, 0.17813f, 0.1995f, 0.69782f, 0.74196f, 0.83796f, 0.52088f, 0.95104f, 0.2453f, 0.92559f, 0.51504f, 0.49455f, 0.29307f, 0.6669f, 0.29431f, 0.63998f, 0.10371f, 0.99132f, 0.39126f, 0.29636f, 0.87779f, 0.39586f, 0.17836f, 0.58076f, 0.87993f, 0.15579f, 0.44186f, 0.37569f, 0.24007f, 0.45984f, 0.10929f, 0.86097f, 0.32595f, 0.59703f, 0.45183f, 0.32153f, 0.38065f, 0.79934f, 0.36841f, 0.42793f, 0.88717f, 0.89446f, 0.68527f, 0.43741f, 0.22042f, 0.38137f, 0.02004f, 0.88971f, 0.31659f, 0.3771f, 0.78324f, 0.37102f, 0.17366f, 0.5573f, 0.27332f, 0.00122f, 0.51681f, 0.51666f, 0.93539f, 0.7004f, 0.87853f, 0.75929f, 0.49979f, 0.51953f, 0.67713f, 0.46579f, 0.38944f, 0.21338f, 0.40531f, 0.87344f, 0.70348f, 0.05289f, 0.91332f, 0.88496f, 0.29694f, 0.34367f, 0.03234f, 0.33708f, 0.19542f, 0.39373f, 0.53637f, 0.61592f, 0.24658f, 0.24479f, 0.81219f, 0.3607f, 0.56095f, 0.58304f, 0.91283f, 0.1531f, 0.29873f, 0.21075f, 0.55407f, 0.652f, 0.77955f, 0.57473f, 0.62495f, 0.50474f, 0.14958f, 0.31533f, 0.90614f, 0.36929f, 0.88973f, 0.08779f, 0.65161f, 0.14016f, 0.95851f, 0.97254f, 0.10756f, 0.0371f, 0.11819f, 0.44102f, 0.1388f, 0.31732f, 0.65871f, 0.44214f, 0.02971f, 0.18419f, 0.02152f, 0.26498f, 0.09404f, 0.38691f, 0.71098f, 0.23038f, 0.29983f, 0.97199f, 0.79792f, 0.7917f, 0.47331f, 0.43053f, 0.65035f, 0.51378f, 0.36919f, 0.35307f, 0.71216f, 0.73997f, 0.91025f, 0.94353f, 0.68866f, 0.06382f, 0.70739f, 0.92547f, 0.02841f, 0.45497f, 0.88709f, 0.03198f, 0.24787f, 0.13855f, 0.30731f, 0.4039f, 0.48707f, 0.93832f, 0.55557f, 0.72668f, 0.76752f, 0.12768f, 0.43994f, 0.21647f, 0.11392f, 0.43086f, 0.01868f, 0.53741f, 0.02908f, 0.46414f, 0.347f, 0.7241f, 0.74915f, 0.24009f, 0.74032f, 0.62324f, 0.43149f, 0.97969f, 0.01622f, 0.84532f, 0.48589f, 0.17123f, 0.38863f, 0.75039f, 0.55292f, 0.94932f, 0.71125f, 0.86689f, 0.68825f
} {

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

	data[i++] = rand[x]; // grad[perm[x] % 16][0];
	data[i++] = 1.0f;
	data[i++] = 1.0f;
	data[i++] = 1.0f;
    }

    randomTable = new Texture2D((GLvoid* )data, 256, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    free(data);

    randomTable->Bind();

    /*
      fix tiling of s and t.
      also, make sure that texture is correctly bound.
     */

    randomTable->SetTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    randomTable->SetMinFilter(GL_LINEAR);
    randomTable->SetMagFilter(GL_LINEAR);

    randomTable->Unbind();
}

void ValueNoiseSeed::Bind(ShaderProgram& shader) {
    shader.SetUniform("permSampler2d", 0);
    shader.SetUniform("permGradSampler", 1);


    Texture::SetActiveTextureUnit(0);
    permTable->Bind();

    Texture::SetActiveTextureUnit(1);
    randomTable->Bind();
}

void ValueNoiseSeed::Unbind() {
    permTable->Unbind();
    randomTable->Unbind();
}
