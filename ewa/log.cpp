#include "log.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <string>

using std::string;

char* logBuffer;

char* errorBuffer;
string errorStr;

void Log(const char* logLevel, const char* file, int line, const char* func, const char* logStr ) {
    fprintf(stderr, "%s: %s:%d:%s:%s\n", logLevel, file,
		 line, func, logStr);
}

void LogInit() {
    logBuffer = new char[1024];
    errorBuffer = new char[1024];
    errorStr = "";

}

void LogDispose() {
    delete [] logBuffer;
    delete [] errorBuffer;
}

void SetError(const char * format, ...) {
    va_list args;
    va_start (args, format);
    vsprintf (errorBuffer, format, args);
    va_end (args);

    errorStr = errorStr + string(errorBuffer) + "\n\n";
}

void SetError(const std::string& str) {
    errorStr = errorStr + str + "\n\n";
}


std::string GetError(){
    return errorStr;
}

void PrintErrorExit() {
    printf("Error: %s\n", GetError().c_str() );
   // exit(1);
	system("pause");
	exit(1);

}
