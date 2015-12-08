#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>


#pragma warning( disable : 4996 )
#pragma warning( disable : 4127 )


void Log(const char* logLevel, const char* file, int line, const char* func, const char* logStr );

void LogInit();
void LogDispose();

extern char* logBuffer;

#define LOG_I(...)							\
    do { sprintf(logBuffer, __VA_ARGS__) ; Log("INFO", __FILE__, __LINE__, __func__, (logBuffer));  } while (0)

#define LOG_E(...)							\
    do { sprintf(logBuffer, __VA_ARGS__); Log("ERROR", __FILE__, __LINE__, __func__, (logBuffer)); system("pause");} while (0)

#define LOG_W(...)							\
    do { sprintf(logBuffer, __VA_ARGS__); ; Log("WARNING", __FILE__, __LINE__, __func__, (logBuffer));  } while (0)

void SetError(const char * format, ...);
void SetError(const std::string& str);

std::string GetError();

void PrintErrorExit();
