#ifndef __RTC_H__
#define __RTC_H__

void rtc_init () ;
void rtc_isr () ;
u64_t rtc_read_timestamp () ;
void rtc_sleep (u32_t ticks) ;

#endif

