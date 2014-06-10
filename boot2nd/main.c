#include <lib.h>
#include <types.h>
#include <i386.h>
#include <term.h>
#include <assert.h>
#include <serial.h>
#include <mem.h>
#include <config.h>
#include <pit.h>
#include <rtc.h>


int main () {
    term_ioctl (TERM_CLEAR,NULL) ;    
	printf ("\a\x07Starting \a\x04IceBox\a\x07 ...\n") ;
	printf ("Reading configuration data...\n") ; 
	config_init () ;
    i386_init () ;
	mem_init () ;
	serial_init () ;
	pci_init () ;
	rtc_init () ;	
	i386_sti () ; 
	
    for (;;) {
		
		rtc_sleep (1000) ;
		printf ("Tick\n") ;
		
	} 
    return 0 ;
}
