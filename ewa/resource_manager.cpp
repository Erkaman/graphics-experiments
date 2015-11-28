#include "resource_manager.hpp"

#include "file.hpp"

#include "log.hpp"

using std::string;

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
    if(!ResourceExists(resourceName, foundResource)) 
		LOG_E("The resource %s could not be found", resourceName.c_str());


	return foundResource;
    
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


std::string ResourceManager::LocateAndReadResource(const std::string& resourcePath) {
    std::string fullResourcePath = ResourceManager::GetInstance().FindResource(resourcePath);

    File f(fullResourcePath, FileModeReading);

    if(f.HasError()) {
	LOG_E("Could not read the resource %s: %s", resourcePath.c_str(), f.GetError().c_str() );
    }

    return f.GetFileContents();
}

File ResourceManager::OpenResourceForReading(const std::string& resourcePath) {

    std::string fullResourcePath = ResourceManager::GetInstance().FindResource(resourcePath);


    File f(fullResourcePath, FileModeReading);

    if(f.HasError()) {
	LOG_E("Could not read the resource %s: %s", resourcePath.c_str(), f.GetError().c_str() );
    }

    return f;
}

ShaderProgram* ResourceManager::LoadShader(
    string vertexShaderPath,
    string fragmentShaderPath,
    std::vector<string> defines) {

    string vertexSource = ResourceManager::LocateAndReadResource(vertexShaderPath);
    string fragmentSource = ResourceManager::LocateAndReadResource(fragmentShaderPath);

    for(size_t i = 0; i < defines.size(); ++i) {
	vertexSource = string("#define ") + defines[i] +  string("\n\n") + vertexSource;
	fragmentSource = string("#define ") + defines[i] +  string("\n\n") + fragmentSource;
    }

    return new ShaderProgram(vertexSource, fragmentSource);
}
