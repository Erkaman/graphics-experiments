#include "shader_program_builder.hpp"
#include "file.hpp"

#include "common.hpp"
#include <vector>
#include "str.hpp"

using namespace std;

GLuint createShaderFromString(const string& str, GLenum shaderType, const string& shaderPath);
string formatCompilerErrorOutput(GLuint shader);

string getLogInfo(GLuint shaderProgram) {
    GLint len;
    GL_C(glGetShaderiv(shaderProgram,  GL_INFO_LOG_LENGTH, &len));

    char* infoLog = new char[len];
    GLsizei actualLen;
    glGetShaderInfoLog(shaderProgram, len, &actualLen, infoLog);
    string logInfoStr(infoLog, actualLen);
    delete infoLog;
    return logInfoStr;
}

bool getCompileStatus(GLuint shaderProgram)  {
    GLint status;
    GL_C(glGetShaderiv(shaderProgram,  GL_COMPILE_STATUS, &status));
    return status == GL_TRUE;
}

ShaderProgramBuilder::ShaderProgramBuilder(const string& vertexShaderPath, const string& fragmentShaderPath) {
    compiledVertexShader = buildAndCompileShader(vertexShaderPath, GL_VERTEX_SHADER);
    compiledFragmentShader = buildAndCompileShader(fragmentShaderPath,  GL_FRAGMENT_SHADER);

    attach();

    bindAttribLocation(VBO_POSITION_ATTRIB_INDEX, "positionIn"); // vertex attribute "position" will have index 0
    bindAttribLocation(VBO_TEX_COORD_ATTRIB_INDEX, "texCoordIn"); //vertex attribute "texCoord" will have index 2
    bindAttribLocation(VBO_NORMAL_ATTRIB_INDEX, "normalIn");//vertex attribute "normal" will have index 1
    bindAttribLocation(VBO_COLOR_ATTRIB_INDEX, "colorIn");

    link();

}

GLuint ShaderProgramBuilder::buildAndCompileShader(const string& shaderPath, GLenum shaderType){

    string shaderContents = "#version 150\n"  + File::GetFileContents(shaderPath) + "\0";

    return createShaderFromString(shaderContents, shaderType, shaderPath);
}


GLuint createShaderFromString(const string& str, GLenum shaderType, const string& shaderPath) {

    GLuint shader;
    // this one is quite old. and it doesn't seem to work.
    GL_C(shader =    glCreateShader(shaderType));

    if(shader == 0) {
	LOG_W("Could not create shader of type %d", shaderType);
	return -1;
    }


    const char* c_str = str.c_str();
    const GLint len = str.size();
    GL_C(glShaderSource(shader, 1, &c_str, &len));
    GL_C(glCompileShader(shader));

    if (!getCompileStatus(shader)) {
	// compilation failed
	LOG_W("Could not compile shader %s\n%s", shaderPath.c_str(), formatCompilerErrorOutput(shader).c_str());
	exit(1);

    }

    return shader;
}


string formatCompilerErrorOutput(GLuint shader)  {
    string result = "";

    vector<string> errors =  SplitString(getLogInfo(shader), "\n");

    // iterate over the errors, one after one.
    for(string error: errors) {


	// print the error.
	result.append(error);
	result.append("\n");
    }

    return result;

}

void ShaderProgramBuilder::attach()  {

    GL_C(shaderProgram = glCreateProgram());

    // attach.
    GL_C(glAttachShader(shaderProgram, compiledFragmentShader));
    GL_C(glAttachShader(shaderProgram, compiledVertexShader));

    // clean up.
    GL_C(glDeleteShader( compiledFragmentShader));
    GL_C(glDeleteShader(compiledVertexShader));
}

void ShaderProgramBuilder::bindAttribLocation(GLuint attribIndex, const string& attribName) {

    GL_C(glBindAttribLocation(this->shaderProgram, attribIndex, attribName.c_str()));
}

void ShaderProgramBuilder::link() {

    GL_C(glLinkProgram(shaderProgram));

    GLint linkOk;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkOk);

    if (linkOk == GL_FALSE) {
	LOG_E("Error linking program: %s", getLogInfo(shaderProgram).c_str());
    }
}
