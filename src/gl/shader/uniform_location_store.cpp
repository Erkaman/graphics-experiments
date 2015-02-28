#include "uniform_location_store.hpp"

using namespace std;

int getActiveUniforms(GLuint shaderProgram);

UniformLocationStore::~UniformLocationStore() {

}

UniformLocationStore::UniformLocationStore(GLuint shaderProgram) {
    int numActiveUniforms = getActiveUniforms(shaderProgram);

    LOG_I("num: "  +tos(numActiveUniforms));

    // length of the returned uniform name.
    GLsizei nameLength;

    // the name is written to this buffer.
    char nameBuffer[256];

    // the size of the OpenGL uniform.
    GLsizei uniformSize;

    // the type of the returned uniform.
    GLenum type;


    for(int i = 0; i < numActiveUniforms; ++i) {
	GL_C(glGetActiveUniform(shaderProgram, i, 256, &nameLength,
			   &uniformSize,
			   &type,
				nameBuffer));

//	String uniformName = BufferUtil.toString(nameBuffer, nameLength.get(0));

	int uniformLocation;
	GL_C(uniformLocation = glGetUniformLocation(shaderProgram, nameBuffer));

	// If the uniform is located in a uniform block, then uniformLocation will be -1.
	if(uniformLocation != -1) {
	    this->uniformLocationStore[string(nameBuffer)] = uniformLocation;
	}
    }
}

/**
 *
 * Returns the number of uniforms in a shader program
 */
int getActiveUniforms(GLuint shaderProgram) {

    GLint result;
    GL_C(glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &result));
    return result;
}

GLuint UniformLocationStore::GetUniformLocation(const string& uniformName) {
    if(uniformLocationStore.count(uniformName) == 1) {
	return uniformLocationStore[uniformName];
    } else
	return GL_NONE;
}
