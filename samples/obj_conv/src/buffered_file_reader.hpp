#pragma once

#include "ewa/file.hpp"

class BufferedFileReader {

private:

    File m_file;
    char* m_buffer;

    size_t m_iBuffer;
    size_t m_readBufferSize;
    bool m_eof;


public:

    BufferedFileReader(const std::string& filename);
    ~BufferedFileReader();

    char Read8();
    bool IsEof()const;

    // read character until a newline is encountered, is eof is reached. The newline is not included in the returned string.
    std::string ReadLine();

};
