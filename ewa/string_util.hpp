#pragma once

#include <string>
#include <vector>

class StringUtil {

public:

    static std::vector<std::string> SplitString(const std::string& str,
					 const std::string& delimiter, const bool keepDelimiters = false);



    // check if str begins with substr.
    static bool BeginsWith(const std::string& str,
		    const std::string& substr);
    static void ToLowercase(std::string& str);

};
