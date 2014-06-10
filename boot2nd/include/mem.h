#ifndef MM_H
#define MM_H

#include <stddef.h>
#include <types.h>

/* mm_init : this call initializes the memory manager by setting memory from 
   start to end to be freed and put in to the memory pool
*/

int 	mem_init () ;
void *  mem_palloc (u32_t size) ;
void    mem_pfree (void *) ;
void    mem_debug () ;



#endif


