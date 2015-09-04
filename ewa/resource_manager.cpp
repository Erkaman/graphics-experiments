#include "resource_manager.hpp"

#include "file.hpp"

#include "log.hpp"
#include "file.hpp"

ResourceManager& ResourceManager::GetInstance() {
    static ResourceManager instance;
    return instance;
}


ResourceManager::ResourceManager() {
    AddResourcePath(".");
}


void ResourceManager::AddResourcePath(const std::string& path) {
    resourcePaths.push_back(path);
}

std::string ResourceManager::FindResource(const std::string& resourceName) {

    std::string foundResource;
    if(!ResourceExists(resourceName, foundResource)) {
	LOG_E("The resource %s could not be found", resourceName.c_str());
    } else {
	return foundResource;
    }
}

bool ResourceManager::ResourceExists(const std::string& resourceName, std::string& foundResource) {

    for(const std::string& path : resourcePaths ) {

	std::string resourcePath = File::AppendPaths(path, resourceName);

	if(File::Exists(resourcePath)) {
	    foundResource = resourcePath;

	    return true;
	}
    }

    foundResource = "";
    return false;
}

bool ResourceManager::ResourceExists(const std::string& resourceName) {
    std::string placeHolder;
    return ResourceExists(resourceName, placeHolder);
}
