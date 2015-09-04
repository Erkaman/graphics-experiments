#include "eob_file.hpp"

#include "common.hpp"

#include "file.hpp"


#define EOBF 0x46424F45

//#define EOBF 0x45 4F 42 46


// https://www.opengl.org/registry/doc/glspec33.core.20100311.withchanges.pdf#page=32
struct FileHeader {
    int32 magic; // Always has the value "EOBF"

    uint32 m_numTriangles; //GLuint is 32-bit
    int32 m_indexType; // GLenum is 32-bit
};

// 32-bit size, then GLvoid data, then cast.

void WriteArray(File& file, void* data, uint32 size) {
    file.Write32u(size);
    file.WriteArray(data, size);
}

void* ReadArray(File& file, uint32& size) {
    size = file.Read32u();
    void* data = malloc(size);

    file.ReadArray(data, size);
    return data;
}

void EobFile::Write(const GeometryObjectData& data, const std::string& outfile) {

    File f(outfile, FileModeWriting);

    FileHeader header;
    header.magic = EOBF;
    header.m_numTriangles = data.m_numTriangles;
    header.m_indexType = data.m_indexType;

    f.WriteArray(&header, sizeof(header));

    WriteArray(f, (void*)&data.m_vertexAttribsSizes[0], sizeof(GLuint) * data.m_vertexAttribsSizes.size() );
    WriteArray(f, data.m_vertices, data.m_verticesSize );
    WriteArray(f, data.m_indices, data.m_verticesSize );


    // file header with data, then write.

    // write size of attirbs, then array

    // do same for vertices
    // do same for indices.
}


GeometryObjectData EobFile::Read(const std::string& infile) {

    GeometryObjectData data;

    File f(infile, FileModeReading);

    FileHeader header;

    f.ReadArray(&header, sizeof(header));

    if(header.magic != EOBF) {
	LOG_E("%s is not a EOB file: invalid magic number %d", infile.c_str(), header.magic );
    }

    data.m_numTriangles = header.m_numTriangles;
    data.m_indexType = header.m_indexType;

    //  WriteArray(f, (void*)&data.m_vertexAttribsSizes[0], sizeof(GLuint) * data.m_vertexAttribsSizes.size() );

    uint32 length = f.Read32u();
    std::vector<GLuint> vertexAttribsSizes;
    vertexAttribsSizes.reserve(length / sizeof(GLuint));
    f.ReadArray(&vertexAttribsSizes[0], length);
    data.m_vertexAttribsSizes = std::vector<GLuint>(&vertexAttribsSizes[0]+0, &vertexAttribsSizes[0]+length / sizeof(GLuint));


    uint32 temp = 0;
    data.m_vertices= ReadArray(f, temp);
    data.m_verticesSize = temp;


    temp = 0;
    data.m_indices= ReadArray(f, temp);
    data.m_indicesSize = temp;

    return data;
}
