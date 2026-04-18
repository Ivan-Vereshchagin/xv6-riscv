#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "spinlock.h"
#include "defs.h"
#include "rtc.h"

struct spinlock rtc_lock;

static volatile uint32 *rtc_low = (uint32*)RTC_LOW;
static volatile uint32 *rtc_high = (uint32*)RTC_HIGH;

void rtc_init(void) {
    initlock(&rtc_lock, "rtc");
}

uint64 rtc_read() {
    uint32 l;
    uint32 h;

    acquire(&rtc_lock);

    l = *rtc_low;
    h = *rtc_high;

    release(&rtc_lock);
    
    return ((uint64)h << 32) | l;
}
