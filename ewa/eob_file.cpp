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

void WriteArray(File& file, void* data, uint64 size) {
    file.Write64u(size);
    file.WriteArray(data, size);
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
