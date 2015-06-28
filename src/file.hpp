#pragma once

#include <string>
#include <stdio.h>

class File {

private:

    // file pointer.
    FILE* m_fp;
    std::string m_filename;

public:
    File(const std::string& filename, const std::string& mode);
    ~File();

    long GetFileSize();
    std::string GetFileContents();

    static std::string GetFileContents(const std::string& filename);
    static bool Exists(const std::string& filename);
};
