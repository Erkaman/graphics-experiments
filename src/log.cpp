#include "log.hpp"

#include <stdio.h>

char* buffer;

void Log(const char* logLevel, const char* file, int line, const char* func, const char* logStr ) {
    fprintf(stderr, "%s: %s:%d:%s:%s\n", logLevel, file,
		 line, func, logStr);
}

void LogInit() {
    buffer = new char[200];
}

void LogDispose() {
    delete [] buffer;
}
