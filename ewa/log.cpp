#include "log.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <string>

char* logBuffer;

char* errorBuffer;

void Log(const char* logLevel, const char* file, int line, const char* func, const char* logStr ) {
    fprintf(stderr, "%s: %s:%d:%s:%s\n", logLevel, file,
		 line, func, logStr);
}

void LogInit() {
    logBuffer = new char[1024];
    errorBuffer = new char[1024];

}

void LogDispose() {
    delete [] logBuffer;
    delete [] errorBuffer;
}

void SetError(const char * format, ...) {
    va_list args;
    va_start (args, format);
    sprintf (errorBuffer, format, args);
    va_end (args);
}

std::string GetError(){
    return errorBuffer;
}

void PrintErrorExit() {
    printf("Error: %s\n", GetError().c_str() );
    exit(1);
}
