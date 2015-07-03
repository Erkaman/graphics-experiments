#include "framework.hpp"

#include <math.h>

constexpr static float EPSILON = 0.00001f;


void AssertEqualsFloat(const float expected, const float actual) {
    AssertEqualsBase( fabs(expected - actual) < EPSILON, std::to_string(expected), std::to_string(actual));
}

void AssertEqualsInt(const int expected, const int actual) {
    AssertEqualsBase(expected == actual, std::to_string(expected), std::to_string(actual));
}


void AssertEqualsBase(const bool equals ,const std::string& expectedStr, const std::string& actualStr) {
    if(! equals ) {
	fprintf(stderr, "  AssertEquals failed for %s == %s\n", expectedStr.c_str(), actualStr.c_str());
	exit(1);
    } else {
	fprintf(stdout, "  AssertEquals passed for %s == %s\n", expectedStr.c_str(), actualStr.c_str());
    }
}

void AssertNotEqualsBase(const bool notEquals ,const std::string& expectedStr, const std::string& actualStr) {
    if(! (notEquals) ) {
	fprintf(stderr, "  AssertNotEquals failed for %s != %s\n", expectedStr.c_str(), actualStr.c_str());
	exit(1);
    } else {
	fprintf(stdout, "  AssertNotEquals passed for %s != %s\n", expectedStr.c_str(), actualStr.c_str());
    }
}

void AssertTrue(const bool b) {
    if(!b) {
	fprintf(stderr, "  AssertTrue failed\n");
	exit(1);
    } else {
	fprintf(stdout, "  AssertTrue passed\n");
    }
}

void AssertNotTrue(const bool b) {
    if(b) {
	fprintf(stderr, "  AssertNotTrue failed\n");
	exit(1);
    } else {
	fprintf(stdout, "  AssertNotTrue passed\n");
    }
}


void BeginSuite(const std::string& suiteName) {
    printf("Begin suite %s\n", suiteName.c_str());
}

void EndSuite(const std::string& suiteName) {
    printf("End suite %s\n", suiteName.c_str());
}

void BeginMethod(const std::string& methodName) {
    printf(" Begin method %s\n", methodName.c_str());
}

void EndMethod(const std::string& methodName) {
    printf(" End method %s\n", methodName.c_str());
}

void RunSuite(const TestSuite& suite, const std::string& suiteName) {

    BeginSuite(suiteName);

    for(const MethodTestPair& pair : suite){
	BeginMethod(pair.second);

	// run the method test.
	pair.first();

	EndMethod(pair.second);

    }

    EndSuite(suiteName);
}

void AssertFailed(const std::string& message) {
    fprintf(stderr, "  Assert failed: %s\n", message.c_str());
    exit(1);
}

void AssertPassed(const std::string& message) {
    fprintf(stderr, "  Assert passed: %s\n", message.c_str());
}
