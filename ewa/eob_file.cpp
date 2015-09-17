#include "eob_file.hpp"

#include "common.hpp"

#include "file.hpp"

using std::string;


#define EOBF 0x46424F45
#define CHNK 0x4B4E4843
#define MATS 0x5354414D

#define VETX 0x58544556
#define INDX 0x58444E49

// https://www.opengl.org/registry/doc/glspec33.core.20100311.withchanges.pdf#page=32
struct FileHeader {
    int32 m_magic; // Always has the value "EOBF"

//    uint32 m_numTriangles; //GLuint is 32-bit
    int32 m_indexType; // GLenum is 32-bit
    int m_hasNormalMaps; // 1 if has normal maps(and also tangents), 0 if not.
};

struct ChunkHeader {
    int32 m_magic; // Always has the value "CHNK"

    uint32 m_numTriangles; //GLuint is 32-bit
};

struct MaterialHeader {
    int32 m_magic; // Always has the value "MATS"
};


// 32-bit size, then GLvoid data, then cast.

void WriteArray(File& file, void* data, uint32 size) {
    file.Write32u(size);
    file.WriteArray(data, size);
}

void WriteString(File& file, const std::string& str) {
    file.Write32u(str.size()+1); // +1, because we want to include the NULL-character as well.
    file.WriteArray(str.c_str(), str.size()+1);
}

std::string ReadString(File& file) {
    uint32 size = file.Read32u();
    char* buffer = (char *)malloc(size * sizeof(char));
    file.ReadArray(buffer, size);
    string str(buffer);
    free(buffer);

    return str;
}

void* ReadArray(File& file, uint32& size) {
    size = file.Read32u();
    void* data = malloc(size);

    file.ReadArray(data, size);
    return data;
}

void EobFile::Write(const GeometryObjectData& data, const std::string& outfile) {

    File f(outfile, FileModeWriting);

    FileHeader fileHeader;
    fileHeader.m_magic = EOBF;
    fileHeader.m_indexType = data.m_indexType;
    fileHeader.m_hasNormalMaps = data.m_hasNormalMaps;

    // first write fileHeader.
    f.WriteArray(&fileHeader, sizeof(fileHeader));

    // next we write vertex attrib sizes.
    WriteArray(f, (void*)&data.m_vertexAttribsSizes[0], sizeof(GLuint) * data.m_vertexAttribsSizes.size() );

    // next write chunk count.

    f.Write32u(data.m_chunks.size());
    for(uint32 i = 0; i < data.m_chunks.size(); ++i) {

	GeometryObjectData::Chunk* chunk = data.m_chunks[i];

	ChunkHeader chunkHeader;

	// write chunk header
	chunkHeader.m_magic = CHNK;
	chunkHeader.m_numTriangles = chunk->m_numTriangles;
	f.WriteArray(&chunkHeader, sizeof(chunkHeader));


	// write chunk material.
	MaterialHeader materialHeader;
	materialHeader.m_magic = MATS;
	f.WriteArray(&materialHeader, sizeof(materialHeader));



	WriteString(f, chunk->m_material.m_textureFilename);
	if(data.m_hasNormalMaps) {
	    WriteString(f, chunk->m_material.m_normalMapFilename);
	}


	// write chunk vertex and index data.
	f.Write32u(VETX);
	WriteArray(f, chunk->m_vertices, chunk->m_verticesSize );
	f.Write32u(INDX);
	WriteArray(f, chunk->m_indices, chunk->m_indicesSize );


    }
}


// TODO: clean up the memory allocated in this method.

// TOOD: ALSO NOTE THAT ONLY UNSIGNED SHORTS ARE HANDLED. UNSIGNED INTS ARE NOT YET HANDLED.
GeometryObjectData EobFile::Read(const std::string& infile) {

    GeometryObjectData data;

    File f(infile, FileModeReading);

    FileHeader fileHeader;

    if(f.HasError()) {
	LOG_E("Could not open the file %s: %s", infile.c_str(), f.GetError().c_str() );
    }

        // TOOD: ALSO NOTE THAT ONLY UNSIGNED SHORTS ARE HANDLED. UNSIGNED INTS ARE NOT YET HANDLED.
    f.ReadArray(&fileHeader, sizeof(fileHeader));

    //LOG_I("init read");

    if(fileHeader.m_magic != EOBF) {
	LOG_E("%s is not a EOB file: invalid magic number %d", infile.c_str(), fileHeader.m_magic );
    }

    data.m_indexType = fileHeader.m_indexType;
    data.m_hasNormalMaps = fileHeader.m_hasNormalMaps;

    // read vertex attrib size array
    uint32 length = f.Read32u();
    std::vector<GLuint> vertexAttribsSizes;
    vertexAttribsSizes.reserve(length / sizeof(GLuint));
    f.ReadArray(&vertexAttribsSizes[0], length);
    data.m_vertexAttribsSizes = std::vector<GLuint>(&vertexAttribsSizes[0]+0, &vertexAttribsSizes[0]+length / sizeof(GLuint));

        // TOOD: ALSO NOTE THAT ONLY UNSIGNED SHORTS ARE HANDLED. UNSIGNED INTS ARE NOT YET HANDLED.


    uint32 numChunks = f.Read32u();
    for(uint32 i = 0; i < numChunks; ++i) {

	GeometryObjectData::Chunk* chunk = new GeometryObjectData::Chunk();
	ChunkHeader chunkHeader;
	f.ReadArray(&chunkHeader, sizeof(chunkHeader));

	if(chunkHeader.m_magic != CHNK) {
	    LOG_E("%s is not a EOB file: chunk number %d has an invalid magic number %d", infile.c_str(), i, chunkHeader.m_magic );
	}

	chunk->m_numTriangles = chunkHeader.m_numTriangles;

	MaterialHeader materialHeader;
	f.ReadArray(&materialHeader, sizeof(materialHeader));
	if(materialHeader.m_magic != MATS) {
	    LOG_E("%s is not a EOB file: chunk number %d has an invalid material magic number %d", infile.c_str(), i, materialHeader.m_magic );
	}

	chunk->m_material.m_textureFilename = ReadString(f);
	if(data.m_hasNormalMaps) {
	    chunk->m_material.m_normalMapFilename = ReadString(f);
	}

	uint32 vertexMagic = f.Read32u();
	if(vertexMagic != VETX) {
	    LOG_E("%s is not a EOB file: chunk number %d has an invalid vertex magic number %d", infile.c_str(), i, vertexMagic);
	}

	uint32 temp = 0;
	chunk->m_vertices= ReadArray(f, temp);
	chunk->m_verticesSize = temp;


	float*vs = (float *) chunk->m_vertices;

/*	for(int i = 0; i < 100; ++i) {
	    LOG_I("v: %f", vs[i]);
	    }*/

	uint32 indexMagic = f.Read32u();
	if(indexMagic != INDX) {
	    LOG_E("%s is not a EOB file: chunk number %d has an invalid index magic number %d", infile.c_str(), i, indexMagic);
	}

	temp = 0;
	chunk->m_indices= ReadArray(f, temp);
	chunk->m_indicesSize = temp;

	data.m_chunks.push_back(chunk);

    }


    return data;
}


/*debugging tip:
draw the colors of the uv on the cube. does the uv map really look like it is supposed to?
*/
