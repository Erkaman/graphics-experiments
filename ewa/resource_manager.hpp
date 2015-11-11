#pragma once

#include <vector>
#include <string>

#include "file.hpp"

// Singelton
class ResourceManager {

private:

    ResourceManager& operator=(const ResourceManager& other);
    ResourceManager(const ResourceManager& other);
    ResourceManager();

    std::vector<std::string> resourcePaths;

public:

    static ResourceManager& GetInstance();

    void AddResourcePath(const std::string& path);


    std::string FindResource(const std::string& resourceName);
    bool ResourceExists(const std::string& resourceName, std::string& foundResource);

    bool ResourceExists(const std::string& resourceName);

    // locate a resource, fully read it into a string, then return the string.
    static std::string LocateAndReadResource(const std::string& resourcePath);

    static File OpenResourceForReading(const std::string& resourcePath);

};
