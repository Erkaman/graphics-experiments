#pragma once

#include <string>
#include <stdio.h>

enum FileMode {
    FileModeReading,
    FileModeWriting
};

class File {

private:

    // file pointer.
    FILE* m_fp;
    std::string m_filename;
    std::string m_errorMessage;

public:
    File(const std::string& filename, const FileMode fileMode );
    ~File();

    long GetFileSize();
    std::string GetFileContents();

    // has an error occurred?
    bool HasError()const;
    std::string  GetError()const;

    static bool Exists(const std::string& filename);

    void WriteArray(const void* data, const size_t dataSize);
    void*  ReadArray(const size_t& dataSize);
    void  Skip(const size_t& skipSize);

    static void WriteArray(const std::string& filename, const void* data, const size_t dataSize);
    static void* ReadArray(const std::string& filename, size_t& dataSize);
};
