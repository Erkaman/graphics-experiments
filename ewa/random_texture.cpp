#include "random_texture.hpp"

#include "math/vector3f.hpp"

#include "log.hpp"

RandomTexture::RandomTexture(GLsizei size, Seed seed):  Texture(GL_TEXTURE_1D) {

    Vector3f* randomData = new Vector3f[size];
//    randomData.reserve(size);

    Random rng(seed);

    for (GLsizei i = 0 ; i < size ; i++) {

	Vector3f v(
	    rng.RandomFloat(),
	    rng.RandomFloat(),
	    rng.RandomFloat()
	    );

	randomData[i] = v;
    }

    Bind();
	GL_C(glTexImage1D(m_target, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, randomData));
    Unbind();

    delete [] randomData;

    Bind();
	SetTextureRepeat();
	SetMinFilter(GL_LINEAR);
	SetMagFilter(GL_LINEAR);
    Unbind();
}
