#ifndef __LIB_H__
#define __LIB_H__

#include <types.h>
#include <stddef.h>
/*  Built-in GCC functions */
void * 	memset (void *,int,size_t) ;
void * 	memcpy (void *,void const *,size_t) ;

/*  */
char * 	ultod (unsigned long,char *,size_t) ;
char * 	ltod (signed long,char *,size_t) ;
char *  utox (u32_t,u64_t,char *,size_t) ;

char * 	strcpy (char *,char const *) ;
char * 	strcat (char *,char const *) ;
size_t 	strlen (char const *) ;

int    	printf (char const *,...) ;
int    	puts (char const *) ;
int    	putchar (int c) ;
char * 	format (char *,char const *,...) ;

void * 	memswap (void *,void *,size_t) ;

char *  strdup (char const *) ;
void *  memdup (void const *,size_t) ;
void * 	malloc (size_t) ;
void * 	calloc (size_t,size_t) ;
void   	free (void *) ;


#endif
