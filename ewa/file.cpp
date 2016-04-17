#include "file.hpp"

#include "resource_manager.hpp"
#include "log.hpp"


#include <errno.h>
#include <string.h>

#ifdef _WIN32

#include "my_dirent.h"

#else // UNIX

#include <sys/stat.h>

#include <dirent.h>


#endif



using namespace std;

// https://msdn.microsoft.com/en-us/library/ttcz0bys.aspx
#pragma warning( disable : 4996 )

const static string PATH_SEPARATOR = "/";


static const char * getError() {
    return strerror(errno);
}

File* File::Load(const std::string& filename, const FileMode fileMode) {

    File* file = new File();
    file->m_fp = NULL;
    file->m_errorMessage = "";


    // create the directory of the file if necessary.
    if(FileModeWriting == fileMode) {

	string path = GetFilePath(filename);

	if(!File::PathExists(path))
	    CreatePath(path);

	file->m_filename = filename;

    } else {

	std::string* resourcePath = ResourceManager::GetInstance().SearchResource(filename);

	if(!resourcePath) {
	    return NULL;
	}
	file->m_filename = *resourcePath;

    }

    file->m_fp = fopen(file->m_filename.c_str(), fileMode == FileModeReading ? "rb" : "wb ");

    if(!file->m_fp) {

	SetError( "could not open the file " + file->m_filename + ":" + std::string(getError())  );

	return NULL;
    }

    return file;
}

File::~File() {
    if(m_fp) {
	if(fclose(m_fp) == EOF) {
	    LOG_E("could not close file %s:%s", m_filename.c_str(),getError());
	}
    }
}

long File::GetFileSize() {

    // seek to end of file.
    fseek(m_fp, 0L, SEEK_END);
    long size = ftell(m_fp);

    // reset file pointer
    fseek(m_fp, 0L, SEEK_SET);

    return size;
}

string File::GetFileContents() {

    long fileSize = this->GetFileSize();

    char* buffer = (char *)malloc(fileSize * sizeof(char));
    fread(buffer, sizeof(char), (size_t)fileSize, m_fp);
    string str(buffer, fileSize);
    free(buffer);

    return str;
}

/*string File::GetFileContents(const string& filename) {
    File f(filename, "rb");
    string contents = f.GetFileContents();
    return contents;
    }*/


bool File::Exists(const std::string& filename) {
	return ResourceManager::GetInstance().ResourceExists(filename);
}


void File::WriteArray(const void* data, size_t dataSize) {
    fwrite(data, 1, dataSize, m_fp);
}

void File::WriteArray(const std::string& filename, const void* data, const size_t dataSize) {
    // writing to a file never fails, so this works
    File::Load(filename, FileModeWriting)->WriteArray(data, dataSize);
}

void* File::ReadArray(const size_t& dataSize) {
    void* ptr = malloc(dataSize);
    fread(ptr, 1, dataSize, m_fp);
    return ptr;
}

size_t File::ReadArray(void* outData, const size_t dataSize) {
    return fread(outData, 1, dataSize, m_fp);
}

void File::Skip(const size_t& skipSize) {
    ReadArray(skipSize);
}

void* File::ReadArray(const std::string& filename, size_t& dataSize) {
    File* file = File::Load(filename, FileModeReading);

    if(!file)
	return NULL;

    dataSize = file->GetFileSize();
    return file->ReadArray(dataSize);
}

bool File::HasError()const {
    return m_errorMessage != "";
}


std::string File::GetError()const {
    return m_errorMessage;
}


string File::GetFilePath(const std::string& filepath) {

    size_t index = filepath.find_last_of(PATH_SEPARATOR);

    if(index == string::npos) {
	return "./"; // it is the current directory.
    } else {
	return filepath.substr(0, index+1);
    }
}

std::string File::AppendPaths(const std::string& path1, const std::string& path2 ) {
    size_t index = path1.find(PATH_SEPARATOR);
    bool includeSeparator;
    if(index == path1.length()-1) {
	includeSeparator = false;
    } else {
	includeSeparator = true;
    }

    return
	path1 +
	(includeSeparator ? PATH_SEPARATOR : "") +
	path2;
}

bool File::PathExists(const std::string& path ) {

#ifdef _WIN32

    // TODO: handle errors.
    DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

#else // UNIX

  struct stat sb;
  return (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));

#endif

}

void CreatePathHelper(const std::string& path) {

#ifdef _WIN32

    // TODO: handle errors.
    CreateDirectory(path.c_str(), NULL);


#else // UNIX

    // TODO: handle error.
    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

#endif
}

void File::CreatePath(const std::string& path) {
    size_t beg = 0;

    do {

	size_t end = path.find("/", beg);
	string p = path.substr(0,end);
	CreatePathHelper(p);
	if(end == string::npos ) {
	    break;
	}

//	LOG_I("p: %s", p.c_str() );

	beg = end+1;

    }while(beg < path.size());
}


void File::Write64u(const uint64 u) {
    WriteArray(&u, sizeof(u));
}

uint64 File::Read64u() {
    uint64 u;
    ReadArray(&u, sizeof(u));
    return u;
}

void File::Write32u(const uint32 u) {
    WriteArray(&u, sizeof(u));
}

uint32 File::Read32u() {
    uint32 u;
    ReadArray(&u, sizeof(u));
    return u;
}


std::string File::GetFile(const std::string& filepath) {
    size_t index = filepath.find_last_of(PATH_SEPARATOR);

    if(index == string::npos) {
	return filepath;
    } else {
	return filepath.substr(index+1, filepath.size() -index);
    }
}


void File::WriteLine(const std::string& str) {
    fputs(str.c_str(), m_fp);
    putc('\n', m_fp);
}

bool File::IsEof() {
    return feof(m_fp) != 0;
}

void File::ReadLine(char* buffer, int bufferSize) {

	//LOG_I("feof: %d", feof(m_fp));

    fgets(buffer, bufferSize, m_fp);
}


std::vector<std::string> File::EnumerateDirectory( std::string path) {

#ifdef _WIN32
	// hack on windows
	path = "../../../samples/game/" + path;
#endif


    DIR* d;
    struct dirent *dir;

    d = opendir(path.c_str());

    vector<string> files;

    if (d)
    {
	while ((dir = readdir(d)) != NULL)
	{
	    files.push_back(string(dir->d_name) );
	}

	closedir(d);
    }

    return files;
}
