#pragma once

#include <string>
#include <stdio.h>

#include "common.hpp"

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
    void ReadArray(void* outData, const size_t& dataSize);

    void  Skip(const size_t& skipSize);


    void Write64u(const uint64 u);
    uint64 Read64u();

    void Write32u(const uint32 u);
    uint32 Read32u();


    static void WriteArray(const std::string& filename, const void* data, const size_t dataSize);
    static void* ReadArray(const std::string& filename, size_t& dataSize);

    // given a filepath(like elias/eric/test.txt), get the directory this file is located in.
    static std::string GetFileDirectory(const std::string& filepath);

    static std::string AppendPaths(const std::string& path1, const std::string& path2 );

    // check whether a path exists.
    static bool PathExists(const std::string& path );

    // recursively create a path.
    static void CreatePath(const std::string& path);



};
