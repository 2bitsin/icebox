#include <i386.h>
#include <pit.h>
#include <pic.h>
#include <lib.h>

#define PIT_REG_CHANNEL_0 	0x40
#define PIT_REG_CHANNEL_1 	0x41
#define PIT_REG_CHANNEL_2 	0x42
#define PIT_REG_COMMAND 	0x43

#define PIT_CHANNEL_0		0 
#define PIT_CHANNEL_1		1 
#define PIT_CHANNEL_2		2 
#define PIT_READ_BACK		3 

#define PIT_TIME_CONSTANT 	838100
#define PIT_TIME_MULTIPLIER	1000000000000LL

u64_t pit_global_time = 0 ;
u64_t pit_global_tadd = 0 ;
u32_t pit_global_rval = 0 ;
u32_t pit_global_freq = 0 ;


u32_t pit_init (u32_t frequency) {
	
	i386_cli () ;
	
	if (frequency <= PIT_MIN_FREQUENCY) {
		frequency = PIT_MIN_FREQUENCY ;	
		pit_global_rval = PIT_MAX_RELOAD_VAL ;
	}
	else if (frequency >= PIT_MAX_FREQUENCY) {
		frequency = PIT_MAX_FREQUENCY ;
		pit_global_rval = PIT_MIN_RELOAD_VAL ;		
	}
	else {
		pit_global_rval = ((PIT_FREQUENCY*3) / frequency)/3 ;
	}
	pit_global_time = 0 ;
	pit_global_freq = frequency ;
	pit_global_tadd = PIT_TIME_CONSTANT * pit_global_rval * 2 ;
		
	outpb (PIT_REG_COMMAND,0x34) ;	
	outpb (PIT_REG_CHANNEL_0,((pit_global_rval-1) >> 0) & 0xff) ;
	outpb (PIT_REG_CHANNEL_0,((pit_global_rval-1) >> 8) & 0xff) ;
		
	return pit_global_freq ;
}

void pit_isr () {	
	pit_global_time += pit_global_tadd ;
	//printf ("ISR TIME %d SEC\r",(u32_t)(pit_global_time / PIT_TIME_MULTIPLIER)) ;
}

