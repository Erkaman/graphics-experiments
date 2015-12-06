#include "buffered_file_reader.hpp"


using std::string;

constexpr size_t BUFFER_SIZE = 4096;


BufferedFileReader::BufferedFileReader(File* file) {

    m_file = file;

    m_buffer = new char[BUFFER_SIZE];

    m_iBuffer = BUFFER_SIZE + 1;
    m_readBufferSize = 0;
    m_eof = false;
}


BufferedFileReader* BufferedFileReader::Load(const std::string& filename) {

    File* file = File::Load(filename, FileModeReading );

    if(!file) {
	return NULL;
    }

    return new BufferedFileReader(file);
}

BufferedFileReader::~BufferedFileReader() {
    delete[] m_buffer;
}


char BufferedFileReader::Read8() {
    if(m_iBuffer == (BUFFER_SIZE + 1) || m_iBuffer==m_readBufferSize) {

	m_readBufferSize = m_file->ReadArray (m_buffer, BUFFER_SIZE);
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


	bool readCr = false; // read carriage return?

    while(true) {

		ch = Read8();

		if (IsEof())
			break;

		if(readCr) {
			if (ch != '\n') {
				LOG_E("cr must followed by lf");
			}
			else {
				break; // else windows new line.
			}
		}

		if (ch == '\n')
			break;



		if (ch == '\r')
			readCr = true;


	if(!readCr) // cr is part of newline, so not part of string.
		str += ch;
    }

    return str;
}
