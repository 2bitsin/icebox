#include <mem.h>
#include <lib.h>
#include <assert.h>
#include <config.h>

#define CHUNK_USED 				0x44455355
#define CHUNK_FREE				0x45455246

typedef struct __chunk_t chunk_t ;

struct __chunk_t {
	u32_t type ;
	u32_t size ;	
	chunk_t * next ;
	chunk_t * prev ;
	u08_t data [] ;
} ;

extern chunk_t __TOP [] ;
static chunk_t * p_root = __TOP ;


int mem_init () {
	memset (p_root,0,sizeof (*p_root)) ;
	p_root->type = CHUNK_FREE ;
	p_root->size = (HEAP_LIMIT + 1)*_1MB - (u32_t)p_root ;
}

void * mem_palloc (u32_t size) {
	chunk_t * p = p_root ;	
	chunk_t * n = NULL ;
	
	u32_t __size = ((size + HEAP_ALIGMENT - 1) / HEAP_ALIGMENT) * HEAP_ALIGMENT + sizeof (chunk_t) ;
	
	assert (__size >= size) ;
	
	while (p) {
		if (p->type == CHUNK_FREE && p->size >= __size + HEAP_ALIGMENT + sizeof (chunk_t))
			break ;
		p = p->next ;		
	}
	
	assert (p != NULL) ;
	
	n = (chunk_t *)((u8_t *)p + p->size - __size) ;
	
	n->type = CHUNK_USED ;
	n->size = __size ;
	n->prev = p ;
	n->next = p->next ;
	if (p->next) 
		p->next->prev = n ;
	p->next = n ;
	p->size = p->size - __size ;
	return &n->data [0] ;
}

static void mem_check_and_merge (chunk_t * n) {
    chunk_t * temp ;
    assert (n->type == CHUNK_FREE) ;
    while (n->prev && n->prev->type == CHUNK_FREE) {
        temp = n->prev ;
        temp->next = n->next ;
        if (n->next)
            n->next->prev = temp ;
        temp->size = n->size + temp->size ;
        n = temp ;
    }
    while (n->next && n->next->type == CHUNK_FREE) {
        temp = n->next ;
        n->next = temp->next ;
        if (temp->next)
            temp->next->prev = n ;
        n->size = temp->size + n->size ;
    }
}

void mem_pfree (void * p) {
	chunk_t * n  = (chunk_t *)p - 1 ;
	n->type = CHUNK_FREE ;	
	mem_check_and_merge (n) ;
}

void mem_debug () {

    chunk_t * p = p_root ;
	printf ("MEM LOG START...\n") ;
    while (p) {
        printf (
            "NODE AT (%x, %s) :\n"
            "\t(%x)::next = %x\n"
            "\t(%x)::prev = %x\n"
            "\t(%x)::size = %d\n"
            "\t(%x)::type = %x\n",
            p, 
            (p->type == CHUNK_USED ? "USED" : 
                (p->type == CHUNK_FREE ? "FREE" : "????")),
            p,p->next,
            p,p->prev,
            p,p->size,
            p,p->type) ;        
        p = p->next ;   
    }
} ;	

