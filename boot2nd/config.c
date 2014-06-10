#include <types.h>
#include <config.h>

static params_t * p_param = 
	(params_t *)ADDR_PARAM_BASE ;

static void mmap_sort () {
	int i = 0,j = 0 ;		
	do {
		j = 0 ;
		for (i = 0;i < p_param->entries-1;++i) {			
			if (p_param->mmap [i].base <= p_param->mmap [i+1].base) 
				continue ;
			memswap (
				&p_param->mmap [i],
				&p_param->mmap [i+1],
				sizeof (smap_t)) ;			
			++j ;
		}
	}
	while (j > 0) ;
	
}

void config_init () {
	mmap_sort () ;
}

