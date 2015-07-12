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


    void WriteArray(const void* data, const size_t dataSize);
    void*  ReadArray(const size_t& dataSize);

    static void WriteArray(const std::string& filename, const void* data, const size_t dataSize);
    static void* ReadArray(const std::string& filename, size_t& dataSize);


};
