#pragma once

void Log(const char* logLevel, const char* file, int line, const char* func, const char* logStr );

void LogInit();
void LogDispose();

extern char* buffer;

#define LOG_I(...)							\
    do { sprintf(buffer, __VA_ARGS__) ; Log("INFO", __FILE__, __LINE__, __func__, (buffer));  } while (0)

#define LOG_E(...)							\
    do { sprintf(buffer, __VA_ARGS__); Log("ERROR", __FILE__, __LINE__, __func__, (buffer)); exit(1); } while (0)

#define LOG_W(...)							\
    do { sprintf(buffer, __VA_ARGS__); ; Log("WARNING", __FILE__, __LINE__, __func__, (buffer));  } while (0)
