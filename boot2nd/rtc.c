#include <i386.h>
#include <rtc.h>
#include <pic.h>
#include <lib.h>

static u64_t rtc_timestamp = 0 ;

void rtc_init () {
	register u08_t r ;
	i386_pushfl () ;
	i386_cli () ;
	
	outpb (0x70,0x8B) ;	
	r = inpb (0x71) ;
	outpb (0x70,0x8B) ;	
	outpb (0x71,0x40|r) ;	
	
	rtc_timestamp =  0LL ;
		
	i386_popfl () ;
}

void rtc_isr () {
	outpb (0x70,0x0C) ;	
	inpb (0x71) ;
	++rtc_timestamp ;
}

u64_t rtc_read_timestamp () {
	return rtc_timestamp ;
}

void rtc_sleep (u32_t ticks) {
	i386_pushfl () ;
	i386_sti () ;	
	u64_t temp = rtc_timestamp + ticks ;
	while (rtc_timestamp < temp) 
		i386_hlt () ;
	i386_popfl () ;	
}
