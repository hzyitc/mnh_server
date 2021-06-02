#include "log.h"

#include <time.h>

const char *timestr() {
	time_t t;
	time(&t);
	struct tm *timeinfo = localtime(&t);
	static char str[120];
	strftime(str, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
	return str;
}