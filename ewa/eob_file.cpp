#include "eob_file.hpp"

#include "common.hpp"

#include "file.hpp"

#include "buffered_file_reader.hpp"


#include "ewa/string_util.hpp"

#include <map>
#include <vector>
#include <assert.h>

using std::string;
using std::map;
using std::vector;


#define EOBF 0x46424F45
#define CHNK 0x4B4E4843
#define MATS 0x5354414D

#define VETX 0x58544556
#define INDX 0x58444E49

struct FileHeader {
    int32 m_magic; // Always has the value "EOBF"

    int32 m_indexType; // GLenum is 32-bit
};

struct ChunkHeader {
    int32 m_magic; // Always has the value "CHNK"

    uint32 m_numTriangles; //GLuint is 32-bit
};

struct MaterialHeader {
    int32 m_magic; // Always has the value "MATS"
};

float StrToFloat(const string& str) {
    string::size_type size;
    return std::stof(str, &size) ;
}

float StrToBool(const string& str) {
    if(str == "true") {
	return true;
    } else {
	// default to false.
	return false;
    }
}

void WriteArray(File* file, void* data, uint32 size) {
    file->Write32u(size);
    file->WriteArray(data, size);
}

void WriteString(File* file, const std::string& str) {
    file->Write32u(str.size()+1); // +1, because we want to include the NULL-character as well.
    file->WriteArray(str.c_str(), str.size()+1);
}

std::string ReadString(File* file) {
    uint32 size = file->Read32u();
    char* buffer = (char *)malloc(size * sizeof(char));
    file->ReadArray(buffer, size);
    string str(buffer);
    free(buffer);

    return str;
}

void* ReadArray(File* file, uint32& size) {
    size = file->Read32u();
    void* data = malloc(size);

    file->ReadArray(data, size);
    return data;
}

bool WriteMaterialFile(const GeometryObjectData& data, const std::string& outfile) {

    // strip ".eob" extension,
    string materialFile = outfile.substr(0, outfile.size()-4 ) + ".mat" ;

    File* f = File::Load(materialFile, FileModeWriting);

    if(!f) {
	return false;
    }

    for(GeometryObjectData::Chunk* chunk : data.m_chunks) {

	Material* mat = chunk->m_material;

	f->WriteLine("new_mat " + mat->m_materialName);

	if(mat->m_textureFilename != "")
	    f->WriteLine("diff_map " + mat->m_textureFilename);

	if(mat->m_normalMapFilename != "")
	    f->WriteLine("norm_map " + mat->m_normalMapFilename);

	if(mat->m_specularMapFilename != "")
	    f->WriteLine("spec_map " + mat->m_specularMapFilename);

	if(mat->m_hasHeightMap)
	    f->WriteLine("has_height_map");

	f->WriteLine("shininess " + std::to_string(mat->m_shininess));

	Vector3f v = mat->m_specularColor;

	f->WriteLine("spec_color "
		     + std::to_string(v.x) + " "
		     + std::to_string(v.y) + " "
		     + std::to_string(v.z));
    }

    return true;
}

bool EobFile::Write(const GeometryObjectData& data, const std::string& outfile) {

    if(outfile.substr(outfile.size()-4, 4 ) != string(".eob") ) {
	SetError("outfile %s is not a EOB file: wrong file extension" );
	return false;
    }

    File* f = File::Load(outfile, FileModeWriting);

    if(!f) {
	return false;
    }

    if(!WriteMaterialFile(data, outfile) ) {
	SetError("Failed to write material file for %s", outfile.c_str() );
	return false;
    }

    if(data.m_indexType != GL_UNSIGNED_SHORT) {

	SetError("We only support storing indices as GLushorts!");
	return false;
    }

    FileHeader fileHeader;
    fileHeader.m_magic = EOBF;
    fileHeader.m_indexType = data.m_indexType;


    // first write fileHeader.
    f->WriteArray(&fileHeader, sizeof(fileHeader));

    // next we write vertex attrib sizes.
    WriteArray(f, (void*)&data.m_vertexAttribsSizes[0], sizeof(GLuint) * data.m_vertexAttribsSizes.size() );

    // next write chunk count.

    f->Write32u(data.m_chunks.size());
    for(uint32 i = 0; i < data.m_chunks.size(); ++i) {

	GeometryObjectData::Chunk* chunk = data.m_chunks[i];

	ChunkHeader chunkHeader;

	// write chunk header
	chunkHeader.m_magic = CHNK;
	chunkHeader.m_numTriangles = chunk->m_numTriangles;
	f->WriteArray(&chunkHeader, sizeof(chunkHeader));


	// write chunk material.
	MaterialHeader materialHeader;
	materialHeader.m_magic = MATS;
	f->WriteArray(&materialHeader, sizeof(materialHeader));

	WriteString(f, chunk->m_material->m_materialName);

	// write chunk vertex and index data.
	f->Write32u(VETX);
	WriteArray(f, chunk->m_vertices, chunk->m_verticesSize );
	f->Write32u(INDX);
	WriteArray(f, chunk->m_indices, chunk->m_indicesSize );
    }

    return true;
}

EntityInfo* ReadEntityInfo(const std::string& infile) {

    string entityFile = infile.substr(0, infile.size()-4 ) + ".ent" ;

    BufferedFileReader* reader = BufferedFileReader::Load(entityFile);

    if(!reader) {
	return NULL;
    }

    EntityInfo* entityInfo = new EntityInfo();

    while(!reader->IsEof()) {

	string line = reader->ReadLine();


	if(line[0] == ' ' || line == string("")  || line[0] == '\0' ) {
	    continue; // empty line, ignore.
	}

	vector<string> tokens =StringUtil::SplitString(line, " ");

	if(tokens[0] == "mass") {
	    entityInfo->m_mass = StrToFloat(tokens[1]);
	} else if(tokens[0] == "static") {
	    entityInfo->m_isStatic = StrToBool(tokens[1]);
	}else{
	    SetError("Undefined line in entity file %s:\n%s", entityFile.c_str(), line.c_str() );
	    return NULL;
	}

    }

    return entityInfo;

}

AABB* ReadAABB(const std::string& infile) {

    string aabbFile = infile.substr(0, infile.size()-4 ) + ".aabb" ;

    BufferedFileReader* reader = BufferedFileReader::Load(  aabbFile);

    if(!reader) {
	return NULL;
    }

    AABB* aabb = new AABB();

    string firstLine = reader->ReadLine();
    vector<string> tokens =StringUtil::SplitString(firstLine, " ");

    if(tokens[0] != "max" || tokens.size() != 4) {
	SetError("%s is not a AABB file: invalid max line", aabbFile.c_str());
	return NULL;
    }
    aabb->max = Vector3f(stof(tokens[1]),stof(tokens[2]),stof(tokens[3]) );


    string secondLine = reader->ReadLine();
    tokens =StringUtil::SplitString(secondLine, " ");

    if(tokens[0] != "min" || tokens.size() != 4) {
	SetError("%s is not a AABB file: invalid min line", aabbFile.c_str());
	return NULL;
    }
    aabb->min = Vector3f(stof(tokens[1]),stof(tokens[2]),stof(tokens[3]) );

    // close reader
    delete reader;

    return aabb;

}



// for every eob filed named XYZ.eob, we store a material file in the same directory.
// It is name XYZ.mat
map<string, Material*>* ReadMaterialFile(const std::string& infile) {


    // strip ".eob" extension,
    string materialFile = infile.substr(0, infile.size()-4 ) + ".mat" ;

    BufferedFileReader* reader = BufferedFileReader::Load(materialFile);
    if(!reader) {
	return NULL;
    }

    map<string, Material*>* matlibPtr = new map<string, Material*>;
    map<string, Material*>& matlib = *matlibPtr;


    Material* currentMaterial = NULL;

    while(!reader->IsEof()) {

	string line = reader->ReadLine();
	vector<string> tokens =StringUtil::SplitString(line, " ");
	string firstToken = tokens[0];

	StringUtil::ToLowercase(firstToken);

	if(firstToken == "new_mat") {
	    assert(tokens.size() == 2);

	    matlib[tokens[1]] = new Material;
	    currentMaterial = matlib[tokens[1]];


	} else if(firstToken == "diff_map") {
	    assert(tokens.size() == 2);

	    currentMaterial->m_textureFilename = tokens[1];
	} else if(firstToken == "norm_map") {
	    assert(tokens.size() == 2);

	    currentMaterial->m_normalMapFilename = tokens[1];
	}else if(firstToken == "spec_map") {
	    assert(tokens.size() == 2);

	    currentMaterial->m_specularMapFilename = tokens[1];
	} else if(firstToken == "has_height_map") {
	    assert(tokens.size() == 1);

	    currentMaterial->m_hasHeightMap = true;

	} else if(firstToken == "shininess") {
	    assert(tokens.size() == 2);
	    currentMaterial->m_shininess = StrToFloat(tokens[1]);
	}else if(firstToken == "spec_color") {
	    assert(tokens.size() == 4);

	    currentMaterial->m_specularColor =
		Vector3f(
		    StrToFloat(tokens[1]),
		    StrToFloat(tokens[2]),
		    StrToFloat(tokens[3]));
	}
    }

    return matlibPtr;
}

CollisionShape* ReadYaml(const std::string& infile, EntityInfo* entityInfo) {

    string yamlFile = infile.substr(0, infile.size()-4 ) + ".yaml" ;

    BufferedFileReader* reader = BufferedFileReader::Load(yamlFile);


    if(!reader) {
	return NULL;
    }

//    AABB* aabb = new AABB();


    // ignore first two lines
    reader->ReadLine();
    reader->ReadLine();

    CollisionShape* collisionShape = new CollisionShape();

    string shapeLine = reader->ReadLine();

    string shape = shapeLine.substr(shapeLine.find(":") + 2);

    if(shape == "Sphere") {
	collisionShape->m_shape = SphereShape;
    } else if(shape == "Box") {
	collisionShape->m_shape = BoxShape;
    } else {

	SetError("Unknown collision shape:\n %s", shapeLine.c_str() );
	return NULL;

    }

    while(!reader->IsEof()) {

	string line = reader->ReadLine();

	vector<string> tokens = StringUtil::SplitString(line, " ");

	if(tokens[0] == "radius:") {
	    collisionShape->m_radius = StrToFloat(tokens[1]);
	} else if(tokens[0] == "half-extents:") {


	    // trim brackets.
	    string trimmed = tokens[1].substr(1, tokens[1].size()-2);

	    vector<string> extentTokens =  StringUtil::SplitString(trimmed, ",");

	    collisionShape->m_halfExtents = Vector3f(
		StrToFloat(extentTokens[0]),
		StrToFloat(extentTokens[1]),
		StrToFloat(extentTokens[2])

		);
	} else {
	    SetError("Unsupported file %s", line.c_str() );
	    return NULL;
	}

//	string shape = shapeLine.substr(shapeLine.find(":") + 2);
    }


    return collisionShape;


}

// TODO: clean up the memory allocated in this method.
GeometryObjectData* EobFile::Read(const std::string& infile) {

    // sanity check.
    if(infile.substr(infile.size()-4, 4 ) != string(".eob") ) {
	SetError("%s is not a EOB file: wrong file extension" );
	return NULL;
    }

    GeometryObjectData* data = new GeometryObjectData();


    /*
      Parse material file.
     */
    map<string, Material*>* matlibPtr = ReadMaterialFile(infile);
    if(!matlibPtr) {
	SetError("Failed to read material file for %s", infile.c_str() );
	return NULL;
    }
    map<string, Material*>& matlib = *matlibPtr;

    EntityInfo* entityInfo = ReadEntityInfo(infile);
    if(!entityInfo) {
	SetError("Failed to read entity info file for %s", infile.c_str() );
	return NULL;
    }
    data->m_entityInfo = entityInfo;

    /*
      Parse AABB file
     */

    AABB* aabbPtr = ReadAABB(infile);
    if(!aabbPtr) {
	return NULL;
    }
    data->aabb = *aabbPtr;

    /*
      Parse collision shape(yaml), if it exists.
     */
    string yamlFile = infile.substr(0, infile.size()-4 ) + ".yaml" ;

    if(File::Exists(yamlFile)) {

	data->m_collisionShape = ReadYaml(infile, data->m_entityInfo);
	if(!data->m_collisionShape) {
	    SetError("Could not parse yaml file: %s", infile.c_str() );
	    return NULL;
	}
	LOG_I("parse yaml: %s", yamlFile.c_str() );
    }


    /*
      Actually parse eob file.
     */


    File* f = File::Load(infile, FileModeReading);


    FileHeader fileHeader;

    if(!f) {
	return NULL;
    }

    f->ReadArray(&fileHeader, sizeof(fileHeader));

    if(fileHeader.m_magic != EOBF) {
	SetError("%s is not a EOB file: invalid magic number %d", infile.c_str(), fileHeader.m_magic );
	return NULL;
    }

    data->m_indexType = fileHeader.m_indexType;

    if(data->m_indexType != GL_UNSIGNED_SHORT) {
	SetError("%s is not a EOB file: We only support storing indices are GLushorts!", infile.c_str() );
	return NULL;
    }

    // read vertex attrib size array
    uint32 length = f->Read32u();
    GLuint* vertexAttribsSizes = new GLuint[length / sizeof(GLuint)];
    f->ReadArray(vertexAttribsSizes, length);
    data->m_vertexAttribsSizes = std::vector<GLuint>(&vertexAttribsSizes[0]+0, &vertexAttribsSizes[0]+length / sizeof(GLuint));
    delete []vertexAttribsSizes;

    uint32 numChunks = f->Read32u();
    for(uint32 i = 0; i < numChunks; ++i) {

	GeometryObjectData::Chunk* chunk = new GeometryObjectData::Chunk();
	ChunkHeader chunkHeader;
	f->ReadArray(&chunkHeader, sizeof(chunkHeader));

	if(chunkHeader.m_magic != CHNK) {
	    SetError("%s is not a EOB file: chunk number %d has an invalid magic number %d", infile.c_str(), i, chunkHeader.m_magic );
	    return NULL;
	}

	chunk->m_numTriangles = chunkHeader.m_numTriangles;

	MaterialHeader materialHeader;
	f->ReadArray(&materialHeader, sizeof(materialHeader));
	if(materialHeader.m_magic != MATS) {
	    SetError("%s is not a EOB file: chunk number %d has an invalid material magic number %d", infile.c_str(), i, materialHeader.m_magic );
	    return NULL;
	}


	string materialName = ReadString(f);

	Material* mat = matlib[materialName];

	if(mat == NULL) {
	    SetError("%s is not a EOB file: The material %s could not be found", infile.c_str(), materialName.c_str());
	    return NULL;
	} else {
	    chunk->m_material = mat;
	}

	uint32 vertexMagic = f->Read32u();
	if(vertexMagic != VETX) {
	    SetError("%s is not a EOB file: chunk number %d has an invalid vertex magic number %d", infile.c_str(), i, vertexMagic);
	    return NULL;
	}

	uint32 temp = 0;
	chunk->m_vertices= ReadArray(f, temp);
	chunk->m_verticesSize = temp;

	uint32 indexMagic = f->Read32u();
	if(indexMagic != INDX) {
	    SetError("%s is not a EOB file: chunk number %d has an invalid index magic number %d", infile.c_str(), i, indexMagic);
	    return NULL;
	}

	temp = 0;
	chunk->m_indices= ReadArray(f, temp);
	chunk->m_indicesSize = temp;

	data->m_chunks.push_back(chunk);

    }


    return data;
}


/*debugging tip:
  draw the colors of the uv on the cube. does the uv map really look like it is supposed to?
*/
