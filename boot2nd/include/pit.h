#ifndef __PIT_H__
#define __PIT_H__

#define PIT_FREQUENCY 		1193181
#define PIT_MAX_RELOAD_VAL 	65536
#define PIT_MIN_RELOAD_VAL	1
#define PIT_MAX_FREQUENCY 	(PIT_FREQUENCY/PIT_MIN_RELOAD_VAL)
#define PIT_MIN_FREQUENCY	(PIT_FREQUENCY/PIT_MAX_RELOAD_VAL)

u32_t pit_init (u32_t) ;
u32_t pit_read_counter () ;
void pit_write_counter (u32_t) ;
void pit_isr () ;

#endif



