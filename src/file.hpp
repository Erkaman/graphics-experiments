#pragma once

#include "precomp.hpp"

class File {

private:

    // file pointer.
    FILE* fp;
    std::string filename;

public:
    File() {}


    int Open(const std::string& filename, const std::string& mode);
    int Close();

    long GetFileSize();
    std::string GetFileContents();


};
