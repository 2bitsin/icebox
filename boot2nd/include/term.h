#ifndef TERM_H
#define TERM_H

#include <types.h>

void term_puts  (char const *) ;
void term_putc  (s32_t) ;
void term_ioctl (u32_t,u32_t *) ;

#define TERM_CLEAR      0x0
#define TERM_SCROLL     0x1
#define TERM_SETTABSIZE 0x2
#define TERM_GETTABSIZE 0x3
#define TERM_SETWIDTH   0x4
#define TERM_GETWIDTH   0x5 
#define TERM_SETHEIGHT  0x6
#define TERM_GETHEIGHT  0x7
#define TERM_SETCURSORX 0x8
#define TERM_GETCURSORX 0x9
#define TERM_SETCURSORY 0xA
#define TERM_GETCURSORY 0xB
#define TERM_SETATTR    0xC
#define TERM_GETATTR    0xD
#define TERM_CURSORSHOW 0xE

#endif
