#pragma once

#include <string>
#include <stdio.h>
#include <vector>

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

    File(){}

public:
    static File* Load(const std::string& filename, const FileMode fileMode );

    ~File();

    long GetFileSize();
    std::string GetFileContents();

    // has an error occurred?
    bool HasError()const;
    std::string  GetError()const;
    bool IsEof();

    static bool Exists(const std::string& filename);

    void WriteArray(const void* data, const size_t dataSize);

    void*  ReadArray(const size_t& dataSize);
    size_t ReadArray(void* outData, const size_t dataSize);

    void  Skip(const size_t& skipSize);

    void Write64u(const uint64 u);
    uint64 Read64u();

    void Write32u(const uint32 u);
    uint32 Read32u();

    // wrapper around fgets.
    void ReadLine(char* buffer, int bufferSize);


    void WriteLine(const std::string& str);

    static void WriteArray(const std::string& filename, const void* data, const size_t dataSize);
    static void* ReadArray(const std::string& filename, size_t& dataSize);

    // given a filepath(like elias/eric/test.txt), get the directory this file is located in(elias/eric)
    static std::string GetFilePath(const std::string& filepath);

    static std::string AppendPaths(const std::string& path1, const std::string& path2 );

    // given a filepath(like elias/eric/test.txt), get strip the path and keep only the file(test.txt)
    static std::string GetFile(const std::string& filepath);

    // check whether a path exists.
    static bool PathExists(const std::string& path );

    // recursively create a path.
    /*
      If given a path "a/b/c", create the directory "a/b/c".
     */
    static void CreatePath(const std::string& path);

    // get all the files in a directory.
    static std::vector<std::string> EnumerateDirectory(const std::string& path);
};
