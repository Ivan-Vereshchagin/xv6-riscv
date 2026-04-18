#ifndef RTC_H
#define RTC_H

#include "types.h"

extern struct spinlock rtc_lock;

void rtc_init(void);

uint64 rtc_read(void);

#endif
