#pragma once

#include <string>
#include <vector>

std::vector<std::string> SplitString(const std::string& str,
				     const std::string& delimiter);


// check if str begins with substr.
bool BeginsWith(const std::string& str,
		const std::string& substr);

// given a filepath, get the directory this file is located in.
std::string GetFileDirectory(const std::string& filepath);

std::string AppendPaths(const std::string& path1, const std::string& path2 );
