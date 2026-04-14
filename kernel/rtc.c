#include "types.h"
#include "riscv.h"
#include "memlayout.h"

static volatile uint32 *rtc_low = (uint32*)RTC_LOW;
static volatile uint32 *rtc_high = (uint32*)RTC_HIGH;

uint64 rtc_read() {
    uint32 l;
    uint32 h;
    l = *rtc_low;
    h = *rtc_high;
    
    return ((uint64)h << 32) | l;
}
