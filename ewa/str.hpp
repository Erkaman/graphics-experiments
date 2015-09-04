#pragma once

#include <string>
#include <vector>

std::vector<std::string> SplitString(const std::string& str,
				     const std::string& delimiter);


// check if str begins with substr.
bool BeginsWith(const std::string& str,
		const std::string& substr);
