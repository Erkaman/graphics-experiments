#include "random_texture.hpp"

#include "math/vector3f.hpp"

RandomTexture::RandomTexture(GLsizei size, Seed seed):  Texture(GL_TEXTURE_1D) {

    std::vector<Vector3f> randomData(size);
    randomData.reserve(size);

    Random rng(seed);

    for (GLsizei i = 0 ; i < size ; i++) {

	randomData.push_back(Vector3f(
				 rng.RandomFloat(),
				 rng.RandomFloat(),
				 rng.RandomFloat()
				 ));
    }

    Bind();
    {
	GL_C(glTexImage1D(m_target, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, &randomData[0]));

	SetTextureRepeat();
	SetMinFilter(GL_LINEAR);
	SetMagFilter(GL_LINEAR);
    }
    Unbind();
}
