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


bool noNormals;

bool generateTangents;


map<string, Material*> ParseMtllib(const string& filename);

int ParseFEntry(const string& entry);

//void CalculateNornmals();

void GenerateTangents();

GeometryObjectData CreateGeometryObjectData();

void PrintHelp();

string basePath;

int main (int argc, char * argv[]) {

    LogInit();


    if(argc != 2 && argc!=3) {

	printf("Not enough arguments specified\n\n");
	PrintHelp();
    }

    string objFilename;

    // set default options first:
    generateTangents = false;

    if(argc == 2) {
	objFilename = string(argv[1]);
    } else if(argc == 3){

	string option = string(argv[1]);

	if(option == string("-t") ) {
	    generateTangents = true;
	} else {
	    LOG_E("Unknown option specified: %s", option.c_str());
	}



	objFilename = string(argv[2]);
    }




    basePath = File::GetFilePath(objFilename);



    BufferedFileReader reader(  objFilename);

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

	    //assert(tokens.size() == 4);

	    if(tokens.size() != 4) {

		LOG_I("%s", tokens[0].c_str());
		LOG_I("%s", tokens[1].c_str());
		LOG_I("%s", tokens[2].c_str());

		LOG_E("fail: %s", line.c_str() );
	    }

	    Vector3f point = Vector3f(stof(tokens[1]),stof(tokens[2]),stof(tokens[3]) );
	    points.push_back(point);

	}else if(firstToken == "vt") {

//	    assert(tokens.size() == 3 );
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

    if(generateTangents) {
	GenerateTangents();
    }

    GeometryObjectData data;

    vector<GLuint> vas{3,2,3}; // point, texcoord, normal
    if(generateTangents) // if tangents are stored, add another size.
	vas.push_back(3); // tangent.

    data.m_vertexAttribsSizes = vas;
    data.m_indexType = GL_UNSIGNED_SHORT;
    data.m_hasNormalMaps = generateTangents;

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

    string f = File::GetFile(objFilename);
    string outfile = File::AppendPaths(basePath, f.substr(0, f.size()-4 ) + ".eob" );

    EobFile::Write(data, outfile);

    LogDispose();

    return 0;
}

map<string, Material*> ParseMtllib(const string& filename) {
    map<string, Material*> mtllib;
    Material* currentMaterial = NULL;

    BufferedFileReader reader(File::AppendPaths(basePath, filename) );

    while(!reader.IsEof()) {

	string line = reader.ReadLine();
	vector<string> tokens =SplitString(line, " ");
	string firstToken = tokens[0];

	ToLowercase(firstToken);

	if(firstToken == "newmtl") {
	    assert(tokens.size() == 2);

	    mtllib[tokens[1]] = new Material;
	    currentMaterial = mtllib[tokens[1]];

	    // default diffuse map is an empty diffuse map.
	    currentMaterial->m_textureFilename = "";
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

    noNormals = false;

    if(slashCount != 2 && slashCount != 1) {

	// we will allow faces only on the form v/vt/vn and v/vt.

	LOG_E("only indices on the form v/vt/vn or v/vt are allowed");
    }

    if(slashCount == 1) {
	noNormals = true;
	LOG_E("no normals in the obj file.");
    }

    vector<string> tokens =SplitString(entry, "/");

    Vector3f point = points[ stof(tokens[0])-1 ];
    Vector2f texCoord = texCoords[ stof(tokens[1])-1 ];


    Vector3f normal;

    if(noNormals) {
	normal = Vector3f(0);
    } else {
	normal = normals[ stof(tokens[2])-1 ];
    }

    point.Add(currentChunk->m_vertices);
    texCoord.Add(currentChunk->m_vertices);
    normal.Add(currentChunk->m_vertices);

    if(generateTangents) {
	// add an empty tangent for now. we will compute it later.
	Vector3f tangent(0);
	tangent.Add(currentChunk->m_vertices);
    }

    GLushort index = indexTable.size();

    indexTable[entry] = index;

    return index;
}

/*
void CalculateNornmals() {

    struct Vertex {
	Vector3f m_point;
	Vector2f m_texCoord;
	Vector3f m_normal;
    };

    map<string, Chunk*>::iterator it;
    for ( it = chunks.begin(); it != chunks.end(); it++ ) {
	Chunk* chunk = it->second;

	// create references for easy access.
	Vertex* vertices = (Vertex*)&chunk->m_vertices[0];
	vector<GLushort >& indices =  chunk->m_indices;

	for(size_t i = 0; i < (indices.size() / 3); ++i) {

	    LOG_I("i: %d", i);

	    Vector3f v1 = vertices[indices[i*3]].m_point;
	    Vector3f v2 = vertices[indices[i*3+1]].m_point;
	    Vector3f v3 = vertices[indices[i*3+2]].m_point;

//	    Vector3f v(-1.000000, -1.000000, -1.000000);

	    LOG_I("v1: %s", string(v1).c_str() );
	    LOG_I("v2: %s", string(v2).c_str() );
	    LOG_I("v3: %s", string(v3).c_str() );

	    Vector3f normal = Vector3f::Cross(v2-v1, v3-v1);

	    LOG_I("normal: %s", string(normal).c_str() );
	    LOG_I("");


	    vertices[indices[i*3]].m_normal += normal;
	    vertices[indices[i*3+1]].m_normal += normal;
	    vertices[indices[i*3+2]].m_normal += normal;

	}

	LOG_I("final normals:");

	for(size_t i = 0; i < (chunk->m_vertices.size() / (sizeof(Vertex)/sizeof(float))  ); i+=1) {
	    vertices[i].m_normal = Vector3f::Normalize(vertices[i].m_normal);

	    // LOG_I("point: %s", string(vertices[i].m_point).c_str() );
	    LOG_I("normal: %s", string(vertices[i].m_normal).c_str() );

	    // normal should be (0,-1,0)

	}

    }
}
*/

void PrintHelp() {
    printf("Usage:\n");
    printf("obj_conv [-t] input-file\n");

    printf("\t-t\tGenerate tangents\n");
}


void GenerateTangents() {


    struct Vertex {
	Vector3f m_point;
	Vector2f m_texCoord;
	Vector3f m_normal;
	Vector3f m_tangent;

    };

    map<string, Chunk*>::iterator it;
    for ( it = chunks.begin(); it != chunks.end(); it++ ) {
	Chunk* chunk = it->second;

	// create references for easy access.
	Vertex* vertices = (Vertex*)&chunk->m_vertices[0];
	vector<GLushort >& indices =  chunk->m_indices;

	for(size_t i = 0; i < (indices.size() / 3); ++i) {

	    Vector3f v1 = vertices[indices[i*3+2]].m_point - vertices[indices[i*3]].m_point;
	    Vector3f v2 = vertices[indices[i*3+1]].m_point - vertices[indices[i*3]].m_point;


	    Vector2f st1 = vertices[indices[i*3+2]].m_texCoord - vertices[indices[i*3]].m_texCoord;
	    Vector2f st2 = vertices[indices[i*3+1]].m_texCoord - vertices[indices[i*3]].m_texCoord;

	    float coef = 1.0f/ (st1.x * st2.y - st2.x * st1.y);

	    Vector3f currentTangent;

	    currentTangent.x = coef * (v1.x * st2.y  + v2.x * -st1.y);
	    currentTangent.y = coef * (v1.y * st2.y  + v2.y * -st1.y);
	    currentTangent.z = coef * (v1.z * st2.y  + v2.z * -st1.y);

	    vertices[indices[i*3]].m_tangent += currentTangent;
	    vertices[indices[i*3+1]].m_tangent += currentTangent;
	    vertices[indices[i*3+2]].m_tangent += currentTangent;
	}

//	LOG_I("final normals:");

	for(size_t i = 0; i < (chunk->m_vertices.size() / (sizeof(Vertex)/sizeof(float))  ); i+=1) {
	    //    vertices[i].m_normal = Vector3f::Normalize(vertices[i].m_normal);


	    vertices[i].m_tangent = Vector3f::Normalize(vertices[i].m_tangent);

	    LOG_I("point: %s", string(vertices[i].m_point).c_str() );
	    LOG_I("tangent: %s", string(vertices[i].m_tangent).c_str() );

	    // normal should be (0,-1,0)
	}

    }
}

/*

    struct Vertex {
	Vector3f m_point;
	Vector2f m_texCoord;
	Vector3f m_normal;
    };

we need to be able to handle also the format

    struct Vertex {
	Vector3f m_point;
	Vector2f m_texCoord;
	Vector3f m_normal;
    Vector3f tangent.
    };*/
