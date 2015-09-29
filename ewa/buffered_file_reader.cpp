#include "buffered_file_reader.hpp"

using std::string;

constexpr size_t BUFFER_SIZE = 4096;

BufferedFileReader::BufferedFileReader(const std::string& filename): m_file(filename, FileModeReading) {
    if(m_file.HasError()) {
	LOG_E("error opening file %s: %s", filename.c_str(), m_file.GetError().c_str());
    }

    m_buffer = new char[BUFFER_SIZE];

    m_iBuffer = BUFFER_SIZE + 1;
    m_readBufferSize = 0;
    m_eof = false;

}

BufferedFileReader::~BufferedFileReader() {
    delete[] m_buffer;
}


char BufferedFileReader::Read8() {
    if(m_iBuffer == (BUFFER_SIZE + 1) || m_iBuffer==m_readBufferSize) {

	m_readBufferSize = m_file.ReadArray (m_buffer, BUFFER_SIZE);
	m_iBuffer = 0;
    }

    char ret = m_buffer[m_iBuffer++];

    if(m_readBufferSize != BUFFER_SIZE && m_iBuffer == m_readBufferSize) {
	m_eof = true;
    }

    return ret;
}

bool BufferedFileReader::IsEof()const {
    return m_eof;
}



std::string BufferedFileReader::ReadLine() {
    string str = "";

    char ch;
    while(!IsEof() && (ch = Read8()) != '\n' ) {
	str += ch;
    }

    return str;
}
