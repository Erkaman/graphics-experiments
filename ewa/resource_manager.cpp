#include "resource_manager.hpp"

#include "str.hpp"

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

    for(const std::string& path : resourcePaths ) {
	std::string resourcePath = AppendPaths(path, resourceName);

	if(File::Exists(resourcePath)) {
	    return resourcePath;
	}
    }

    LOG_E("The resource %s could not be found", resourceName.c_str());

}
