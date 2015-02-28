#include "fileutil.hpp"
#include "file.hpp"



using namespace std;

string GetFileContents(const string& filename) {
    File f;
    f.Open(filename, "r");
    string contents = f.GetFileContents();
    f.Close();
    return contents;
}

bool FileExists(const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "r");
    if (fp != NULL)
    {
	fclose(fp);
	printf("File exists\n");
	return true;
    } else {
	return false;
    }
}
