#include <term.h>
#include <i386.h>

static void clear           (void) ;
static void scroll_down     (u32_t) ;
static void put_char        (s32_t) ;
static void show_cursor     (u32_t) ;
static void update_cursor   (u32_t,u32_t) ;

static u16_t * console      = (u16_t *)0xB8000L ;
static u32_t width          = 80L ;
static u32_t height         = 25L ;
static u32_t attr           = 0x7L ;
static u32_t cursor_x       = 0L ;
static u32_t cursor_y       = 0L ;
static u32_t tab_size       = 4L ;
static u32_t cursor_s       = (u32_t)-1L ;
static u32_t escape 		= 0 ;

#define ESCAPE_CLEAR		0
#define ESCAPE_COLOR		1

void clear (void) {
    register u32_t i ;
    for (i = 0;i < width*height;++i) {
        console [i] = (attr << 8) | ' ' ;
    }
    cursor_x = 0L ;
    cursor_y = 0L ;
    if (cursor_s != 0) {
        update_cursor (cursor_x,cursor_y) ;                        
    }
}

void scroll_down (u32_t n) {  
    register u32_t i ;
    if (n < height) {
        for (i = 0;i < width*(height - n);++i) {
            console [i] = console [i+width*n] ;
        }
        for (;i < width*height;++i) {
            console [i] = (attr << 8) | ' ' ;
        }
        cursor_y = height - 1L ;
        cursor_x = 0L ;
    }
    else {
        clear () ;
    }
    if (cursor_s != 0) {
        update_cursor (cursor_x,cursor_y) ;                        
    }
}

void update_cursor (u32_t x,u32_t y) {
    register u32_t i = x + y*width ;
    outb (0x3D4,0xE) ;
    outb (0x3D5,i >> 8) ;
    outb (0x3D4,0xF) ;
    outb (0x3D5,i & 0xff) ;
} 

void show_cursor (u32_t i) {
    register u32_t k ;
    outb (0x3D4,0xA) ;
    k = inb (0x3D5) ;
    if (i != 0) {
        i &= ~0x20 ;
    }
    else {
        i |= 0x20 ;
    }
    outb (0x3D5,i) ;
}

void put_char (s32_t c) {
    u32_t x,y,i ;
		
	if (escape != ESCAPE_CLEAR) {
		switch (escape) {
			case ESCAPE_COLOR:
				term_ioctl (TERM_SETATTR,(u32_t *)&c) ;
				escape = ESCAPE_CLEAR ;
				return ;
		}	
	}
	
	if (c == '\n') 
		put_char ('\r') ;
    switch (c) {
        case '\n' : {
            cursor_y = cursor_y + 1L ; 
            cursor_x = 0L ;
            if (cursor_y >= height) {
                scroll_down (1L) ;
            }
            break ;
        }
        case '\r' : {
            cursor_x = 0L ;
            break ;
        }
        case '\t' : {
            cursor_x = (cursor_x/tab_size + 1)*tab_size ;
            if (cursor_x >= width) {
                x = cursor_x%width ;
                y = cursor_x/width ;
                for (i = 0;i < y;++i) {
                    put_char ('\n') ;
                }
                cursor_x = cursor_x + x ;
            }
            break ;
        }
		case '\a' : {
			escape = ESCAPE_COLOR ;
			return ;
		}
        default : {
            console [cursor_y*width + cursor_x] = (attr << 8) | (c & 0xff) ;
            cursor_x = cursor_x + 1L ;
            if (cursor_x >= width) {
                put_char ('\n') ;
            }
            break ;
        }
    }
    if (cursor_s != 0) {
        update_cursor (cursor_x,cursor_y) ;                        
    }
}


void term_puts (char const * s) {
    while (*s != '\0') {
        put_char (*s++) ;
    }
}

void term_putc (s32_t c) {
    put_char (c) ;
}

void term_ioctl (u32_t n,u32_t * i) {
    switch (n) {
        case TERM_CLEAR :
            clear () ;
            break ;
        case TERM_SCROLL :
            scroll_down (*i) ;
            break ;
        case TERM_SETTABSIZE :
            tab_size = *i ;
            break ;
        case TERM_GETTABSIZE :
            *i = tab_size ;
            break ;
        case TERM_SETWIDTH :
            width = *i ; 
            break ;            
        case TERM_GETWIDTH :
            *i = width ;            
            break ;
        case TERM_SETHEIGHT :
            height = *i ; 
            break ;
        case TERM_GETHEIGHT :
            *i = height ;
            break ;
        case TERM_SETCURSORX :
            cursor_x = *i % width ;
            update_cursor (cursor_x,cursor_y) ;
            break ;
        case TERM_SETCURSORY :
            cursor_y = *i % width ;
            update_cursor (cursor_x,cursor_y) ;
            break ;
        case TERM_GETCURSORX :
            *i = cursor_x ;
            break ;
        case TERM_GETCURSORY :
            *i = cursor_y ;
            break ;
        case TERM_SETATTR :
            attr = *i ;
            break ;
        case TERM_GETATTR :
            *i = attr ;
            break ;
        case TERM_CURSORSHOW :
            if (i != 0) {
                cursor_s = *i ;
            }
            show_cursor (cursor_s) ;
            break ;
    }
}
