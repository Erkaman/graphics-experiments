#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <utility>


constexpr static float EPSILON = 0.0001f;


void AssertEqualsBase(bool equals ,const std::string& expectedStr, const std::string& actualStr) {
    if(! equals ) {
	fprintf(stderr, "  AssertEquals failed for %s == %s\n", expectedStr.c_str(), actualStr.c_str());
	exit(1);
    } else {
	fprintf(stdout, "  AssertEquals passed for %s == %s\n", expectedStr.c_str(), actualStr.c_str());
    }
}

template<typename T>
void AssertEquals(const T& expected, const T& actual) {
    AssertEqualsBase(expected == actual, std::string(expected), std::string(actual));
}

void AssertEquals(const float expected, const float actual) {
    AssertEqualsBase( fabs(expected - actual) < EPSILON, std::to_string(expected), std::to_string(actual));
}


void AssertNotEqualsBase(bool notEquals ,const std::string& expectedStr, const std::string& actualStr) {
    if(! (notEquals) ) {
	fprintf(stderr, "  AssertNotEquals failed for %s != %s\n", expectedStr.c_str(), actualStr.c_str());
	exit(1);
    } else {
	fprintf(stdout, "  AssertNotEquals passed for %s != %s\n", expectedStr.c_str(), actualStr.c_str());
    }
}

template<typename T>
void AssertNotEquals(const T& expected, const T& actual) {
    AssertNotEqualsBase(expected != actual, std::string(expected), std::string(actual));
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


typedef void (*MethodTest)();
typedef std::pair<MethodTest,std::string> MethodTestPair;
typedef std::vector<MethodTestPair> TestSuite;

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
