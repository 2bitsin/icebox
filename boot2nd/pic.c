#include <i386.h>
#include <pic.h>

void pic_remap_irq (u32_t m,u32_t s) {
    outpb (0x20,0x11) ;
    outpb (0xa0,0x11) ;
    outpb (0x21,m) ;
    outpb (0xa1,s) ;
    outpb (0x21,0x4) ;
    outpb (0xa1,0x2) ;
    outpb (0x21,0x1) ;
    outpb (0xa1,0x1) ;        
}


void pic_mask_irq (u32_t n,u32_t s) {
    u32_t x = 1 << n ;
    u32_t o = 0 ;o |= inpb (0x21) ;
    o |= inpb (0xa1) << 8 ;

    if (s != 0) 
        x = o & ~x ;
    else
        x = o | x ;
    outpb (0x21,x & 0xff) ;
    outpb (0xa1,x >> 8) ;
}

void pic_master_ack () {
    outpb (0x20,0x20) ;
}

void pic_slave_ack () {
    outpb (0xa0,0x20) ;
}

void pic_eoi (u32_t irq) {
	pic_master_ack () ;
	if (irq >= 8)
		pic_slave_ack () ; 
}



