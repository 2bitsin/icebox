#include <types.h>
#include <keyb.h>
#include <i386.h>
#include <lib.h>

void keyb_isr () {
	u08_t c = inpb (0x60) ;

}



