#include "file.hpp"

#include <errno.h>
#include "log.hpp"
#include "common.hpp"

using namespace std;

// https://msdn.microsoft.com/en-us/library/ttcz0bys.aspx
#pragma warning( disable : 4996 )

const char * getError() {
    return strerror(errno);
}

File::File(const std::string& filename, const std::string& mode):m_fp(fopen(filename.c_str(), mode.c_str())), m_filename(filename) {

    if(!m_fp) {
	Error("could not open file " + m_filename + ":" + getError());
    }
}

File::~File() {
    if(m_fp) {
	if(fclose(m_fp) == EOF) {
	    Error("could not close file " + m_filename + ":" + getError());
	}
    }
}

long File::GetFileSize() {

    // seek to end of file.
    fseek(m_fp, 0L, SEEK_END);
    long size = ftell(m_fp);

    // reset file pointer
    fseek(m_fp, 0L, SEEK_SET);

    return size;
}

string File::GetFileContents() {

    long fileSize = this->GetFileSize();

    char* buffer = (char *)malloc(fileSize * sizeof(char));
    fread(buffer, sizeof(char), (size_t)fileSize, m_fp);
    string str(buffer, fileSize);
    free(buffer);

    return str;
}


string File::GetFileContents(const string& filename) {
    File f(filename, "r");
    string contents = f.GetFileContents();
    return contents;
}

bool File::Exists(const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "r");
    if (fp != NULL)
    {
	fclose(fp);
	return true;
    } else {
	return false;
    }
}
