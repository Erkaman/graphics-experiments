#include "str.hpp"

using std::string;


const static string PATH_SEPARATOR = "/";

std::vector<std::string> SplitString(const std::string& str,
                                      const std::string& delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}

bool BeginsWith(const std::string& str,
		const std::string& substr) {
    return substr == str.substr(0, substr.length());
}

string GetFileDirectory(const std::string& filepath) {

    size_t index = filepath.find(PATH_SEPARATOR);

    if(index == string::npos) {
	return "./"; // it is the current directory.
    } else {
	return filepath.substr(0, index+1);
    }
}

std::string AppendPaths(const std::string& path1, const std::string& path2 ) {
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
