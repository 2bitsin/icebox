#include <stddef.h>
#include <stdarg.h>
#include <lib.h>
#include <types.h>
#include <i386.h>
#include <mem.h>

void * memswap (void * _a,void * _b,size_t sz) {
	register char * a = (char *)_a;
	register char * b = (char *)_b;
	int i = 0 ;
	for (i = 0;i < sz;++i) {
		register t = a [i] ;
		a [i] = b [i] ;
		b [i] = t ;
	}
	return _a ;
}

void * memset (void * s,int c,size_t n) {
    register char * _s = s ;
    while (n-- > 0)
        *_s++ = c ;
    return s ;
}
void * memcpy (void * s2,void const * s1,size_t n) {
    register char const * _s1 = s1 ;
    register char * _s2 = s2 ;
    while (n-- > 0)
        *_s2++ = *_s1++ ;
    return s2 ;
}

void * calloc (size_t num,size_t size) {	
	return memset (malloc (size*num),0,num*size) ;
}

void * memdup (void const * p,size_t size) {	
	return memcpy (malloc (size),p,size) ;
}

char * strdup (char const * s) {
	return (char *)memdup (s,strlen (s)) ;
}

char * strcpy (char * s1,char const * s2) {
    register char * s = s1 ;
    while (*s2 != '\0') {
        *s1 = *s2 ;
        ++s1 ;
        ++s2 ;
    }
    *s1 = '\0' ;
    return s ;
}

char * strcat (char * s1,char const * s2) {
    register char * s = s1 ;
    while (*s1 != '\0') {
        ++s1 ;
    }
    strcpy (s1,s2) ;
    return s ;
}
size_t strlen (char const * s) {
    register u32_t i = 0L ;
    while (s [i] != '\0') {
        ++i ;
    }
    return i ;
}

char * ultod (unsigned long v,char * d,size_t s) {
    unsigned long m = 1000000000L ;
    char * p = d ;
    if (!v) {
        strcpy (d,"0") ;
        return d ;
    }
    while (m > 0 && s > 1) {
        if (m <= v) 
            *p++ = (v / m) % 10 + '0';
        m /= 10 ;
        --s ;
    }
    if (s > 0) 
        *p = 0 ;
    return d ;
}
char * ltod (long v,char * d,size_t s) {
    if (v < 0) {
        *d++ = '-' ;
        --s ;
        v = -v ;
    }
    ultod (v,d,s) ;
    return d - 1 ;
}

char __xchar (u08_t v) {
	v = v&0xf ;
	if (v < 10)
		return '0' + v ;
	return v - 10 + 'A' ;
}

char * utox (u32_t bits,u64_t v,char * b,size_t s) {
    char * p = b ;
    
    u32_t m = bits ;

    if (s > 8L) {
        while (m > 0L) {
            *b = __xchar ((v >> (m - 4L)) & 0xfL) ;
            ++b ;
            m -= 4L ;
        }
        *b = '\0' ;
    }
    return p ;	
}

#define u32tod ultod
#define s32tod ltod
#define u08tox(v,b,s) utox(8,v,b,s)
#define u16tox(v,b,s) utox(16,v,b,s)
#define u32tox(v,b,s) utox(32,v,b,s)
#define u64tox(v,b,s) utox(64,v,b,s)

int va_format (char * s,char const * f,va_list a) {
    static char v [2048] ;
    union {
        u32_t u32 ;
        s32_t s32 ;
        char const * str ;
		u64_t u64 ;
    } t ;
    *s = 0 ;
    while (*f != '\0') {
        if (*f == '%' && *(f + 1) != '\0') {
            ++f ;
            switch (*f) {
                case 'i' :
                case 'd' :
                case 'l' : {
                    *s = '\0' ;
                    t.s32 = va_arg (a,s32_t) ;
                    s32tod (t.s32,v,sizeof (v)) ;
                    strcat (s,v) ;
                    s += strlen (v) - 1 ;
                    break ;
                }
                case 'u' : {
                    *s = '\0' ;
                    t.u32 = va_arg (a,u32_t) ;
                    u32tod (t.u32,v,sizeof (v)) ;
                    strcat (s,v) ;
                    s += strlen (v) - 1 ;
                    break ;
                }
                case 'b' : {
                    *s = '\0' ;
                    t.u32 = va_arg (a,u32_t) ;
                    u08tox (t.u32,v,sizeof (v)) ;
                    strcat (s,v) ;
                    s += strlen (v) - 1 ;
                    break ;
                }
                case 'w' : {
                    *s = '\0' ;
                    t.u32 = va_arg (a,u32_t) ;
                    u16tox (t.u32,v,sizeof (v)) ;
                    strcat (s,v) ;
                    s += strlen (v) - 1 ;
                    break ;
                }
                case 'x' : {
                    *s = '\0' ;
                    t.u32 = va_arg (a,u32_t) ;
                    u32tox (t.u32,v,sizeof (v)) ;
                    strcat (s,v) ;
                    s += strlen (v) - 1 ;
                    break ;
                }
                case 'X' : {
                    *s = '\0' ;
                    t.u64 = va_arg (a,u64_t) ;
                    u64tox (t.u64,v,sizeof (v)) ;
                    strcat (s,v) ;
                    s += strlen (v) - 1 ;
                    break ;
                }
                case 's' : {
                    *s = '\0' ;
                    t.str = va_arg (a,char const *) ;
                    strcat (s,t.str) ;
                    s += strlen (t.str) - 1 ;
                    break ;
                }
                case 'c' : {
                    *s = va_arg (a,int) ;
                    break ;
                }
                default : {
                    *s = *f ;
                    break ;
                }
            }
        }
        else {
            *s = *f ;
        }
        ++s ;
        ++f ;
    }
    *s = '\0' ;
    return 0 ;
}

int printf (char const * f,...) {
    int r ;
    char v [2048] ;
    va_list i ;
    va_start (i,f) ;
    r = va_format (v,f,i) ;
    term_puts (v) ;
    va_end (i) ;
    return r ;
}

int puts (char const * p) {
    term_puts (p) ;
    term_putc ('\n') ;
    return 1 ;
}

int putchar (int c) {
    term_putc (c) ;
}

void assert_m (int i,char const * p) {
    if (!i) {
        printf ("booti386.sys failed assertion : %s\n",p) ;
        i386_cli () ;
		i386_hlt () ;
    }        
}

void * malloc (size_t size) {
	return mem_palloc (size) ;
}

void free (void * p) {
	mem_pfree (p) ;
}

