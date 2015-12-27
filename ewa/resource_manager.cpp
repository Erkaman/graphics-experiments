#include "resource_manager.hpp"

#include "file.hpp"

#include "log.hpp"

using std::string;

// ResourceManager::GetInstance().FindResource(filename).c_str()
bool FileExists(const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (fp != NULL)
    {
	fclose(fp);
	return true;
    } else {
	return false;
    }
}


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

std::string* ResourceManager::SearchResource(const std::string& resourceName) {

    std::string foundResource;

    if(!ResourceExists(resourceName, foundResource)) {
	SetError("The resource %s could not be found", resourceName.c_str());
	return NULL;
    }

    return new string(foundResource);

}

bool ResourceManager::ResourceExists(const std::string& resourceName, std::string& foundResource) {

    for(const std::string& path : resourcePaths ) {

	std::string resourcePath = File::AppendPaths(path, resourceName);

	if(FileExists(resourcePath)) {
	    foundResource = resourcePath;

	    return true;
	}
    }

    foundResource = "";
    return false;
}


bool ResourceManager::PathExists(const std::string& path) {

	for (const std::string& upperPath : resourcePaths) {

		std::string resourcePath = File::AppendPaths(upperPath, path);

		if (File::PathExists(resourcePath)) {
			return true;
		}
	}

	return false;
}

bool ResourceManager::ResourceExists(const std::string& resourceName) {
    std::string placeHolder;
    return ResourceExists(resourceName, placeHolder);
}


std::string ResourceManager::LocateAndReadResource(const std::string& resourcePath) {
    std::string* fullResourcePath = ResourceManager::GetInstance().SearchResource(resourcePath);

    if(!fullResourcePath) {
	PrintErrorExit();
    }

    File *f = File::Load(resourcePath, FileModeReading);

    if(!f) {
    	PrintErrorExit();
    }

    return f->GetFileContents();
}

File* ResourceManager::OpenResourceForReading(const std::string& resourcePath) {

    std::string* fullResourcePath = ResourceManager::GetInstance().SearchResource(resourcePath);

    if(!fullResourcePath) {
	return NULL;
    }

    File *f = File::Load(resourcePath, FileModeReading);

    if(!f) {
	return NULL;
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

    return ShaderProgram::Load(vertexSource, fragmentSource);
}
