#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <utility>

void AssertEqualsBase(const bool equals ,const std::string& expectedStr, const std::string& actualStr);

template<typename T>
void AssertEquals(const T& expected, const T& actual) {
    AssertEqualsBase(expected == actual, std::string(expected), std::string(actual));
}

void AssertEqualsFloat(const float expected, const float actual);
void AssertEqualsInt(const int expected, const int actual);

void AssertNotEqualsBase(const bool notEquals ,const std::string& expectedStr, const std::string& actualStr);

template<typename T>
void AssertNotEquals(const T& expected, const T& actual) {
    AssertNotEqualsBase(expected != actual, std::string(expected), std::string(actual));
}


typedef void (*MethodTest)();
typedef std::pair<MethodTest,std::string> MethodTestPair;
typedef std::vector<MethodTestPair> TestSuite;

void RunSuite(const TestSuite& suite, const std::string& suiteName);
