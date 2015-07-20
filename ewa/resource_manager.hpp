#pragma once

#include <vector>
#include <string>

// Singeltin
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
};
