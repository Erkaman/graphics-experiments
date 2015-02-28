#include "file.hpp"

using namespace std;

const char * getError() {
    return strerror(errno);
}

int File::Open(const std::string& filename, const std::string& mode) {

    this->filename = filename;

    fp = fopen(filename.c_str(), mode.c_str());
    if(!fp) {
	LOG_E("could not open file " + filename + ":" + getError());
	return -1;
    }

    return 0;
}

int File::Close() {
    if(fp) {
	if(fclose(fp) == EOF) {
	    LOG_E("could not close file " + this->filename + ":" + getError());
	    return -1;
	}
    }
    return 0;

}

long File::GetFileSize() {

    // seek to end of file.
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    // reset file pointer
    fseek(fp, 0L, SEEK_SET);

    return size;
}

string File::GetFileContents() {

    long fileSize = this->GetFileSize();

    char* buffer = (char *)malloc(fileSize * sizeof(char));
    fread(buffer, sizeof(char), (size_t)fileSize, fp);
    string str(buffer, fileSize);
    free(buffer);

    return str;
}
