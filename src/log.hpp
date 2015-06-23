#pragma once

inline void log(const char* logLevel, const char* file, int line, const char* func, const std::string& logStr ) {
    fprintf(stderr, "%s: %s:%d:%s:%s\n", logLevel, file,
		 line, func, (logStr.c_str()));
}

#define LOG_I(x)							\
    do {  log("INFO", __FILE__, __LINE__, __func__, (x));  } while (0)

#define LOG_E(x)							\
    do {  log("ERROR", __FILE__, __LINE__, __func__, (x));  } while (0)

#define LOG_W(x)							\
    do {  log("INFO", __FILE__, __LINE__, __func__, (x));  } while (0)
