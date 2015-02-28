#pragma once

#include "precomp.hpp"

/**
 * Creates a store that stores all locations of all uniforms in a shader program.
 * This way, you can access the locations using this class, instead of having to call the slow openGL call
 *  glGetUniformLocation every time you wish to change a uniform.
 *
 *The store should be created just after the shader has been linked, because of the reasons given below:
 * http://stackoverflow.com/a/20417594
 *
 */
class UniformLocationStore {

private:

    std::map<std::string, GLuint>  uniformLocationStore;

public:

    UniformLocationStore(GLuint shaderProgram);
    ~UniformLocationStore();

    GLuint GetUniformLocation(const std::string& uniformName);

};
