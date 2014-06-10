#ifndef I386_H
#define I386_H

#include <types.h>

typedef
    struct {
        u16_t limit ;
        void * base ;
    }
    treg_t ;
typedef
    struct {
        u32_t
            eax,ebx,ecx,edx,
            edi,esi,ebp,esp,
            ds,es,fs,gs,ss,
            eflags,inum ;
    }
    iexec_t ;

#define EFLAGS_CF       0x00001
#define EFLAGS_TF       0x00100
#define EFLAGS_IF       0x00200
#define EFLAGS_OF       0x00800
#define EFLAGS_NT       0x04000
#define EFLAGS_VM       0x20000


#define EFLAGS_IOPL(X)  ((X) << 12)

void i386_init () ;


/*  **************************************************************
    Misc functions
    ************************************************************** */

static inline u16_t i386_get_cs () {
    u16_t cs ;
    asm ("movw %%cs,%0" : "=r"(cs)) ;
    return cs ;
}

static inline u16_t i386_get_ds () {
    u16_t ds ;
    asm ("movw %%ds,%0" : "=r"(ds)) ;
    return ds ;
}

static inline u32_t i386_get_flags () {
	u32_t flags ;
	asm ("pushfl") ;
	asm ("popl %eax" : "=a"(flags)) ;
	return flags ;
}

static inline u32_t i386_get_cr (int i) {
    u32_t cr ;
    switch (i) {
        case 0 :
            asm ("movl %%cr0,%0" : "=a"(cr)) ;
            return cr ;
        case 2 :
            asm ("movl %%cr2,%0" : "=a"(cr)) ;
            return cr ;
        case 3 :
            asm ("movl %%cr3,%0" : "=a"(cr)) ;
            return cr ;
        case 4 :
            asm ("movl %%cr4,%0" : "=a"(cr)) ;
            return cr ;
        default : 
            return 0 ;
    }
}

static inline void i386_lidt (void * base,u16_t limit) {
    treg_t idtr = {limit,base} ;
    asm ("lidt %0" : : "m"(idtr)) ;
}

static inline void i386_lgdt (void * base,u16_t limit) {
    treg_t gdtr = {limit,base} ;
    asm ("lgdt %0" : : "m"(gdtr)) ;
}

static inline void i386_sgdt (treg_t * treg) {
    asm ("sgdt (%0)" : : "r"(treg)) ;
}

static inline void i386_sidt (treg_t * treg) {
    asm ("sidt (%0)" : : "r"(treg)) ;
}

static inline void i386_ltr (u16_t tr) {
    asm ("ltr %0" : : "r"(tr)) ;
}

static inline void outpb (u16_t p,u8_t v) {
    asm ("outb %0,%1" :: "a"(v), "d"(p)) ;
}

static inline u8_t inpb (u16_t p) {
    u8_t v ;
    asm ("inb %1,%0" : "=a"(v) : "d"(p)) ;
    return v ;
}

static inline void outpw (u16_t p,u16_t v) {
    asm ("outw %0,%1" :: "a"(v), "d"(p)) ;
}

static inline u16_t inpw (u16_t p) {
    u16_t v ;
    asm ("inw %1,%0" : "=a"(v) : "d"(p)) ;
    return v ;
}

static inline void outpl (u16_t p,u32_t v) {
    asm ("outl %0,%1" :: "a"(v), "d"(p)) ;
}

static inline u32_t inpl (u16_t p) {
    u32_t v ;
    asm ("inl %1,%0" : "=a"(v) : "d"(p)) ;
    return v ;
}

#define i386_sti() 		asm ("sti") 
#define i386_cli() 		asm ("cli")  
#define i386_hlt() 		asm ("hlt") 
#define i386_pushfl() 	asm ("pushfl")
#define i386_popfl() 	asm ("popfl")


#define outb outpb
#define inb inpb


#endif
