#include "ewa/log.hpp"

#include "ewa/gl/gl_common.hpp"
#include "ewa/string_util.hpp"

#include "ewa/geometry_object_data.hpp"
#include "ewa/eob_file.hpp"

#include "buffered_file_reader.hpp"

#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector2f.hpp"

#include <map>
#include <assert.h>

using std::vector;
using std::string;
using std::stof;
using std::map;

struct Chunk {
    vector<GLushort> m_indices;
    GLuint m_numTriangles;

    Material* m_material;
};



/*
  Global variables
 */

map<string, GLushort> indexTable;

vector<Vector3f> points;
vector<Vector2f> texCoords;
vector<Vector3f> normals;

vector<float> globalVertices;


Chunk* currentChunk;
map<string, Chunk*>  chunks;
map<string, Material*> mtllib;
bool generateTangents;

string basePath;

/*
  Function declarations
 */

static float StrToFloat(const string& str) {
    string::size_type size;
    return std::stof(str, &size) ;
}

// parse the material library of the .obj-file
map<string, Material*> ParseMtllib(const string& filename);

int ParseFEntry(const string& entry);

// compute the tangents of the model.
void GenerateTangents();

// print command line help.
void PrintHelp();

int main (int argc, char * argv[]) {

    LogInit();

    if(argc != 2 && argc!=3) {
	printf("Not enough arguments specified\n\n");
	PrintHelp();
	exit(1);
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

    // the path where all output files will be put.
    basePath = File::GetFilePath(objFilename);

    BufferedFileReader* reader = BufferedFileReader::Load(  objFilename);
    if(!reader) {
	PrintErrorExit();
    }

    currentChunk = NULL;

    while(!reader->IsEof()) {

	string line = reader->ReadLine();

	vector<string> tokens =StringUtil::SplitString(line, " ");

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

	    assert(
		tokens.size() == 3 ||
		tokens.size() == 4 // if there is a third uv-coordinate, ignore it. we dont support it.
		);

	    Vector2f point = Vector2f(stof(tokens[1]),stof(tokens[2]) );
	    texCoords.push_back(point);

	}else if(firstToken == "vn") {

	    assert(tokens.size() == 4);
	    Vector3f normal = Vector3f(stof(tokens[1]),stof(tokens[2]),stof(tokens[3]) );
	    normals.push_back(normal);

	}else if(firstToken == "f") {

	    GLushort one = (GLushort)ParseFEntry(tokens[1]);
	    GLushort two = (GLushort)ParseFEntry(tokens[2]);

	    for(size_t i = 3; i < tokens.size(); ++i) {
		GLushort three = (GLushort)ParseFEntry(tokens[i]);

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

    /*
      With that, we have parsed the .obj file.
      Now we transfer all the data of the .obj to a GeometryObjectData object.

      Then we can use the engine to output the GeometryObjectData object to a file.
     */

    GeometryObjectData data;

    /*
      Vertex Attributes Sizes.
     */
    vector<GLuint> vas{3,2,3}; // point(3 floats), texcoord(2 floats), normal(3 floats)
    if(generateTangents) // if tangents are stored, add another size.
	vas.push_back(3); // tangent.

    data.m_vertexAttribsSizes = vas;

    // all indices are specified as GLushorts.
    data.m_indexType = GL_UNSIGNED_SHORT;

    data.m_vertices = &globalVertices[0];
    data.m_verticesSize = globalVertices.size() * sizeof(float);

    /*
	newChunk->m_vertices = &baseChunk->m_vertices[0];
	newChunk->m_verticesSize = baseChunk->m_vertices.size() * sizeof(float);
    */

    map<string, Chunk*>::iterator it;
    for ( it = chunks.begin(); it != chunks.end(); it++ ) {
	Chunk* baseChunk = it->second;

	GeometryObjectData::Chunk* newChunk = new GeometryObjectData::Chunk;

	newChunk->m_material = baseChunk->m_material;

	newChunk->m_numTriangles = baseChunk->m_numTriangles;


	newChunk->m_indices = &baseChunk->m_indices[0];
	newChunk->m_indicesSize = baseChunk->m_indices.size() * sizeof(GLushort);

	data.m_chunks.push_back(newChunk);
    }

    string f = File::GetFile(objFilename);
    string outfile = File::AppendPaths(basePath, f.substr(0, f.size()-4 ) + ".eob" );

    if(!EobFile::Write(data, outfile)) {
	PrintErrorExit();
    }

    string cmd = "source ~/.bashrc && bash -c ";
    cmd += "\"";
    cmd += "aabb_create " + objFilename;
    cmd += "\"";

    system( cmd.c_str() );


    LogDispose();

    return 0;
}

map<string, Material*> ParseMtllib(const string& filename) {
    Material* currentMaterial = NULL;

    BufferedFileReader* reader = BufferedFileReader::Load(File::AppendPaths(basePath, filename) );

    while(!reader->IsEof()) {

	string line = reader->ReadLine();
	vector<string> tokens =StringUtil::SplitString(line, " ");
	string firstToken = tokens[0];

	StringUtil::ToLowercase(firstToken);

	if(firstToken == "newmtl") {
	    assert(tokens.size() == 2);

	    mtllib[tokens[1]] = new Material;
	    currentMaterial = mtllib[tokens[1]];


	    currentMaterial->m_materialName = tokens[1];

	} else if(firstToken == "map_kd") {
	    assert(tokens.size() == 2);

	    currentMaterial->m_textureFilename = tokens[1];
	} else if(firstToken == "map_bump") {
	    assert(tokens.size() == 2);

	    if(!generateTangents) {
		LOG_E("This model has normal maps, but tangents are not being generated. Please add the switch -t");
	    }

	    currentMaterial->m_normalMapFilename = tokens[1];
	}else if(firstToken == "map_spec") {
	    assert(tokens.size() == 2);

	    currentMaterial->m_specularMapFilename = tokens[1];
	}else if(firstToken == "kd") { // diffuse color

	    assert(tokens.size() == 4);

	    currentMaterial->m_diffuseColor =
		Vector3f(
		    StrToFloat(tokens[1]),
		    StrToFloat(tokens[2]),
		    StrToFloat(tokens[3]));

	}else if(firstToken == "ks") { // specular color.

	    assert(tokens.size() == 4);

	    currentMaterial->m_specularColor =
		Vector3f(
		    StrToFloat(tokens[1]),
		    StrToFloat(tokens[2]),
		    StrToFloat(tokens[3]));

	}else if(firstToken == "ns") { // specular exponent.
	    assert(tokens.size() == 2);

	    currentMaterial->m_specularExponent = StrToFloat(tokens[1]);



	}/*else if(firstToken == "ke") { // emissiveColor

}*/
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

    if(slashCount != 2 && slashCount != 1) {
	// we will allow faces only on the form v/vt/vn and v/vt.

	LOG_E("only indices on the form v/vt/vn or v/vt are allowed: %s", entry.c_str() );


    }


    if(slashCount == 1) {
	LOG_E("no normals in the obj file. This is not handled by this program");
    }

    vector<string> tokens =StringUtil::SplitString(entry, "/");

    if(points.size() == 0) {
	LOG_E("Found f(ace), but no points have been specified yet");
    }

    Vector3f point = points[ stoi(tokens[0])-1 ];

    if(texCoords.size() == 0) {
	LOG_E("Found f(ace), but no texcoords have been specified yet");
    }

    Vector2f texCoord = texCoords[ stoi(tokens[1])-1 ];


    Vector3f normal;


    normal = normals[ stoi(tokens[2])-1 ];

    point.Add(globalVertices);
    texCoord.Add(globalVertices);
    normal.Add(globalVertices);

    /*
    LOG_I("points: %s", string(point).c_str() );
    LOG_I("texcoord: %s", string(texCoord).c_str() );
    LOG_I("normal: %s", string(normal).c_str() );
    LOG_I("");
*/


    if(generateTangents) {
	// add an empty tangent for now. we will compute it later.
	Vector3f tangent(0);
	tangent.Add(globalVertices);
    }

    const size_t GLushortMax = (GLushort)-1;

    if(indexTable.size() >= GLushortMax ) {
	LOG_E("There are more indices than will fit into a GLushort: %ld", indexTable.size()   );
    }

    GLushort index = (GLushort)indexTable.size();

    indexTable[entry] = index;

    return index;
}

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

    // generate tangents for all the chunks.
    for ( it = chunks.begin(); it != chunks.end(); it++ ) {
	Chunk* chunk = it->second;

	// create references for easy access.
	Vertex* vertices = (Vertex*)&globalVertices[0];
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

	for(size_t i = 0; i < (globalVertices.size() / (sizeof(Vertex)/sizeof(float))  ); i+=1) {
	    vertices[i].m_tangent = Vector3f::Normalize(vertices[i].m_tangent);
	}

    }
}
