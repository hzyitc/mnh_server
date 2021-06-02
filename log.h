#include <stdio.h>

#ifndef __log_h__
#define __log_h__

const char *timestr();
#define log_d(format, ...) fprintf(stdout, "[%s][%s:%d] " format "\n", timestr(), __FILE__, __LINE__, ##__VA_ARGS__)
#define log_i(format, ...) fprintf(stdout, "[%s][%s:%d] " format "\n", timestr(), __FILE__, __LINE__, ##__VA_ARGS__)
#define log_e(format, ...) fprintf(stderr, "[%s][%s:%d] " format "\n", timestr(), __FILE__, __LINE__, ##__VA_ARGS__)

#endif