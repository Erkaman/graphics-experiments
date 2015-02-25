#pragma once

#ifdef _DEBUG
#define LOG_I(fmt, x)							\
    do { fprintf(stderr, "INFO: %s:%d:%s:%s\n", __FILE__,	\
			      __LINE__, __func__, (x)); } while (0)

#define LOG_E(fmt, x)							\
    do { fprintf(stderr, "ERROR: %s:%d:%s:%s\n", __FILE__,	\
			      __LINE__, __func__, (x)); } while (0)


#endif
