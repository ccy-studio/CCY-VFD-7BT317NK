#ifndef __UTIL_H
#define __UTIL_H

#include <time.h>
#include "constant.h"

typedef struct tm esp_time;

long map(long x, long in_min, long in_max, long out_min, long out_max);
esp_time* ntp_async();

#endif