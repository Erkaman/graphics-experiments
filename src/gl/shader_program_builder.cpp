#include "shader_program_builder.hpp"
#include "file.hpp"

#include "common.hpp"
#include <vector>
#include "str.hpp"

using namespace std;

GLuint CreateShaderFromString(const string& str, GLenum shaderType, const string& shaderPath);
string FormatCompilerErrorOutput(const GLuint shader, const string& shaderStr);

string GetLogInfo(GLuint shaderProgram);
bool GetCompileStatus(GLuint shaderProgram);

string ParseShader(const std::string& shaderPath);

ShaderProgramBuilder::ShaderProgramBuilder(const string& vertexShaderPath, const string& fragmentShaderPath, const string& geometryShaderPath) {
	m_compiledVertexShader = BuildAndCompileShader(vertexShaderPath, GL_VERTEX_SHADER);
	m_compiledFragmentShader = BuildAndCompileShader(fragmentShaderPath,  GL_FRAGMENT_SHADER);

    
	if(geometryShaderPath != "") {

		m_compiledGeometryShader = BuildAndCompileShader(geometryShaderPath,  GL_GEOMETRY_SHADER);
	m_hasGeometryShader = true;
    } else {
	m_hasGeometryShader = false;
    }
	LOG_I("attching fragment");

    Attach();

    BindAttribLocation(VBO_POSITION_ATTRIB_INDEX, "positionIn"); // vertex attribute "position" will have index 0
    BindAttribLocation(VBO_TEX_COORD_ATTRIB_INDEX, "texCoordIn"); //vertex attribute "texCoord" will have index 2
    BindAttribLocation(VBO_NORMAL_ATTRIB_INDEX, "normalIn");//vertex attribute "normal" will have index 1
    BindAttribLocation(VBO_COLOR_ATTRIB_INDEX, "colorIn");

    Link();

}

GLuint ShaderProgramBuilder::BuildAndCompileShader(const string& shaderPath, const GLenum shaderType){

    string shaderContents = ParseShader(shaderPath);


    return CreateShaderFromString(shaderContents, shaderType, shaderPath);
}


GLuint CreateShaderFromString(const string& str, const GLenum shaderType, const string& shaderPath) {

    GLuint shader;
    // this one is quite old. and it doesn't seem to work.
    GL_C(shader =    glCreateShader(shaderType));

    if(shader == 0) {
		LOG_E("Could not create shader of type %d", shaderType);
    }


    const char* c_str = str.c_str();
    const GLint len = str.size();
    GL_C(glShaderSource(shader, 1, &c_str, &len));
    GL_C(glCompileShader(shader));

    if (!GetCompileStatus(shader)) {
	// compilation failed
	LOG_W("Could not compile shader %s\n%s", shaderPath.c_str(), FormatCompilerErrorOutput(shader, str).c_str());
	exit(1);
    }

    return shader;
}

string FormatCompilerErrorOutput(const GLuint shader, const string& shaderStr)  {
    string result = "";

    vector<string> errors =  SplitString(GetLogInfo(shader), "\n");

    vector<string> shaderLines = SplitString(shaderStr, "\n");


    // iterate over the errors, one after one.
    for(string error: errors) {

	if(error == "")
	    continue;

	size_t firstColonIndex = error.find(":");
	size_t secondColonIndex = error.find(":", firstColonIndex+1);
	size_t thirdColonIndex = error.find(":", secondColonIndex+1);

	int lineNumber = std::stoi(error.substr(secondColonIndex+1, thirdColonIndex-(secondColonIndex+1)));

	// print the error.
	result.append(error);
	result.append("\nLine where the error occurred:\n");
	result.append(shaderLines[lineNumber-1]);

	// the affected line.
    }

    return result;

}

void ShaderProgramBuilder::Attach()  {

    GL_C(m_shaderProgram = glCreateProgram());

    // attach.
    GL_C(glAttachShader(m_shaderProgram, m_compiledFragmentShader));
    GL_C(glAttachShader(m_shaderProgram, m_compiledVertexShader));
    if(m_hasGeometryShader) {
	GL_C(glAttachShader(m_shaderProgram, m_compiledGeometryShader));
    }

    // clean up.
    GL_C(glDeleteShader( m_compiledFragmentShader));
    GL_C(glDeleteShader(m_compiledVertexShader));
	if (m_hasGeometryShader) {

    GL_C(glDeleteShader(m_compiledGeometryShader));
	}
}

void ShaderProgramBuilder::BindAttribLocation(const GLuint attribIndex, const string& attribName) {

    GL_C(glBindAttribLocation(m_shaderProgram, attribIndex, attribName.c_str()));
}

void ShaderProgramBuilder::Link() {

    GL_C(glLinkProgram(m_shaderProgram));

    GLint linkOk;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linkOk);

    if (linkOk == GL_FALSE) {
	LOG_E("Error linking program: %s", GetLogInfo(m_shaderProgram).c_str());
    }
}


GLuint ShaderProgramBuilder::GetLinkedShaderProgram() {
    return m_shaderProgram;
}

bool GetCompileStatus(GLuint shaderProgram)  {
    GLint status;
    GL_C(glGetShaderiv(shaderProgram,  GL_COMPILE_STATUS, &status));
    return status == GL_TRUE;
}

string GetLogInfo(GLuint shaderProgram) {
    GLint len;
    GL_C(glGetShaderiv(shaderProgram,  GL_INFO_LOG_LENGTH, &len));

    char* infoLog = new char[len];
    GLsizei actualLen;
    glGetShaderInfoLog(shaderProgram, len, &actualLen, infoLog);
    string logInfoStr(infoLog, actualLen);
    delete infoLog;
    return logInfoStr;
}

string ParseShader(const std::string& shaderPath) {

    string parsedShader = "";

    parsedShader +=  "#version 150\n";

    vector<string> shaderLines = SplitString(File::GetFileContents(shaderPath), "\n");

    for(const string& line: shaderLines) {

	if(BeginsWith(line, "#include")) {

	    string includePath = line.substr(10, line.length()-11 );
	    string shaderDir = GetFileDirectory(shaderPath);

	    string includeStr = File::GetFileContents(AppendPaths(shaderDir, includePath));

		parsedShader += includeStr + "\n";
	} else {
	    parsedShader += line + "\n";
	}
    }

    return parsedShader;
}
