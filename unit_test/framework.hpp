#pragma once

#include <stdio.h>
#include <string>

template<typename T>
void AssertEquals(const T& t1, const T& t2) {
    if(! (t1 == t2) ) {
	fprintf(stderr, "  AssertEquals failed for %s == %s\n", std::string(t1).c_str(), std::string(t2).c_str());
	exit(1);
    } else {
	fprintf(stdout, "  AssertEquals passed for %s == %s\n", std::string(t1).c_str(), std::string(t2).c_str());
    }
}

template<typename T>
void AssertNotEquals(const T& t1, const T& t2) {
    if(! (t1 != t2) ) {
	fprintf(stderr, "  AssertNotEquals failed for %s != %s\n", std::string(t1).c_str(), std::string(t2).c_str());
	exit(1);
    } else {
	fprintf(stdout, "  AssertNotEquals passed for %s != %s\n", std::string(t1).c_str(), std::string(t2).c_str());
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
