#include "ewa/log.hpp"

#include "ewa/gl/gl_common.hpp"
#include "ewa/str.hpp"

#include "ewa/geometry_object_data.hpp"
#include "ewa/eob_file.hpp"

#include "buffered_file_reader.hpp"

#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector2f.hpp"

#include <map>


using std::vector;
using std::string;
using std::stof;
using std::map;

// we:
// a buffered file parser that allows reading a single file.


struct Chunk {
    vector<float> m_vertices;
    vector<GLushort> m_indices;

    GLuint m_numTriangles;

    Material* m_material;
};

// TODO: should it not be GLuint?
map<string, GLushort> indexTable;

vector<Vector3f> points;
vector<Vector2f> texCoords;
vector<Vector3f> normals;
Chunk* currentChunk;
map<string, Chunk*>  chunks;
map<string, Material*> mtllib;

map<string, Material*> ParseMtllib(const string& filename);

int ParseFEntry(const string& entry);

GeometryObjectData CreateGeometryObjectData();

int main (int, char *[]) {

    LogInit();

    LOG_I("obj conv");

    BufferedFileReader reader("cube.obj");



    currentChunk = NULL;


    while(!reader.IsEof()) {

	string line = reader.ReadLine();

	vector<string> tokens =SplitString(line, " ");

	string firstToken = tokens[0];

	if(firstToken == "mtllib") {

	    assert(tokens.size() == 2);
	    mtllib = ParseMtllib(tokens[1]);
	}else if(firstToken == "usemtl") {
	    assert(tokens.size() == 2);

	    if(mtllib.size() == 0) {
		LOG_E("invalid usage of usemtl: the mtllib has not yet been loaded, or was empty");
	    }

	    string materialStr = tokens[1];

	    if(mtllib.find(materialStr) == mtllib.end()) {
		    LOG_E("invalid usage of usemtl: the material %s could not be found", materialStr.c_str() );
	    }

	    if(chunks.find(materialStr) == chunks.end()) { // the chunk for the material has not yet been created.

		chunks[materialStr] = new Chunk;

		currentChunk = chunks[materialStr];

		// initialize the chunk.
		currentChunk->m_numTriangles = 0;
		currentChunk->m_material = mtllib[materialStr];


	    } else {
		currentChunk = chunks[materialStr];
	    }
	}else if(firstToken == "v") {

	    assert(tokens.size() == 4);
	    Vector3f point = Vector3f(stof(tokens[1]),stof(tokens[2]),stof(tokens[3]) );
	    points.push_back(point);

	}else if(firstToken == "vt") {

	    assert(tokens.size() == 3);
	    Vector2f point = Vector2f(stof(tokens[1]),stof(tokens[2]) );
	    texCoords.push_back(point);

	}else if(firstToken == "vn") {

	    assert(tokens.size() == 4);
	    Vector3f normal = Vector3f(stof(tokens[1]),stof(tokens[2]),stof(tokens[3]) );
	    normals.push_back(normal);

	}else if(firstToken == "f") {

	    GLushort one = ParseFEntry(tokens[1]);
	    GLushort two = ParseFEntry(tokens[2]);

	    for(size_t i = 3; i < tokens.size(); ++i) {
		GLushort three = ParseFEntry(tokens[i]);

		// add the indices of a single triangle
		currentChunk->m_indices.push_back(one);
		currentChunk->m_indices.push_back(two);
		currentChunk->m_indices.push_back(three);
		++currentChunk->m_numTriangles;

		two = three;
	    }

	}
    }





    GeometryObjectData data;

    data.m_vertexAttribsSizes = vector<GLuint>{3,2,3}; // point, texcoord, normal
    data.m_indexType = GL_UNSIGNED_SHORT;


    map<string, Chunk*>::iterator it;
    for ( it = chunks.begin(); it != chunks.end(); it++ ) {
	Chunk* baseChunk = it->second;

	GeometryObjectData::Chunk* newChunk = new GeometryObjectData::Chunk;

	newChunk->m_material = *baseChunk->m_material;

	newChunk->m_numTriangles = baseChunk->m_numTriangles;

	newChunk->m_vertices = &baseChunk->m_vertices[0];
	newChunk->m_verticesSize = baseChunk->m_vertices.size() * sizeof(float);

	newChunk->m_indices = &baseChunk->m_indices[0];
	newChunk->m_indicesSize = baseChunk->m_indices.size() * sizeof(GLushort);

	data.m_chunks.push_back(newChunk);
    }

    EobFile::Write(data, "dat/cube.eob");


    LogDispose();

    return 0;
}

map<string, Material*> ParseMtllib(const string& filename) {
    map<string, Material*> mtllib;
    Material* currentMaterial = NULL;

    BufferedFileReader reader(filename);

    while(!reader.IsEof()) {

	string line = reader.ReadLine();
	vector<string> tokens =SplitString(line, " ");
	string firstToken = tokens[0];

	ToLowercase(firstToken);

	if(firstToken == "newmtl") {
	    assert(tokens.size() == 2);

	    mtllib[tokens[1]] = new Material;
	    currentMaterial = mtllib[tokens[1]];
	} else if(firstToken == "map_kd") {
	    assert(tokens.size() == 2);

	    currentMaterial->m_textureFilename = tokens[1];
	}
    }

    return mtllib;
}


int ParseFEntry(const string& entry) {

    // if has already been parsed, simply return index.
    if(indexTable.find(entry) != indexTable.end()) {
	return indexTable[entry];
    }

    // count number of forward slashes.
    size_t slashCount = std::count(entry.begin(), entry.end(), '/');

    if(slashCount != 2) {
	LOG_E("only indices on the form v/vt/vn are allowed");
    }

    vector<string> tokens =SplitString(entry, "/");

    Vector3f point = points[ stof(tokens[0])-1 ];
    Vector2f texCoord = texCoords[ stof(tokens[1])-1 ];
    Vector3f normal = normals[ stof(tokens[2])-1 ];

    point.Add(currentChunk->m_vertices);
    texCoord.Add(currentChunk->m_vertices);
    normal.Add(currentChunk->m_vertices);

    GLushort index = indexTable.size();

    indexTable[entry] = index;

    return index;
}
