#include <assert.h>
#include <lib.h>
#include <term.h>
#include <types.h>
#include <i386.h>
#include <pic.h>
#include <keyb.h>
#include <pit.h>
#include <rtc.h>

#define JUMPS       256
#define JMPSZ       17
#define GDESCS      8

#define GD_NULL     0x0000
#define GD_CODE     0x0008
#define GD_DATA     0x0010

#define rtol(S,O,T) ((T *)((((S) & 0xffff) << 4) + ((O) & 0xffff)))
#define trace(...) printf (__VA_ARGS__) 


/*  **************************************************************
    Misc structures
    ************************************************************** */

typedef
    struct {
        u16_t offset_lsw ;
        u16_t selector ;
        u16_t flags ;
        u16_t offset_msw ;
    }
    idesc_t ;

typedef
    struct {u16_t a,b,c,d ;} gdesc_t ;

typedef
    struct {
        u16_t limit0 ;
        u16_t base0 ;
        u8_t base1 ;
        u8_t type ;
        u8_t limit1 ;
        u8_t base2 ;
    }
    tdesc_t ;


typedef
    struct {
        u32_t
            ss,gs,fs,es,ds,
            edi,esi,edx,ecx,ebx,eax,ebp,esp,
            inum,errorcode,
            eip,cs,eflags,
            old_esp,old_ss,
            old_es,old_ds,
            old_fs,old_gs ;
    }
    isr_t ;
	
typedef
    struct {
         u32_t
            backlink,
            esp0,ss0,
            esp1,ss1,
            esp2,ss2,
            cr3,
            eip,eflags,
            eax,ecx,edx,ebx,
            esp,ebp,esi,edi,
            es,cs,ss,ds,fs,gs,
            ldt ;
        u16_t t,iomap ;
    }
    tss_t ;

/*  **************************************************************
    Misc data
    ************************************************************** */

static u32_t * ivt = (u32_t *)0 ;
static u8_t jumps [JUMPS*JMPSZ] ;   /*  Buffer for jump code */
static idesc_t idt [JUMPS] ;        /*  Interrupt descriptor table */
static gdesc_t gdt [GDESCS] ;       /*  Global descriptor table */
extern long i386_isr_ptr ;          /*  Interrupt service rouitine address */

void i386_exception (isr_t * isr) ;

/*  ************************************************************** 
    Creates an entry point for an interrupt service routine
    ************************************************************** */

void i386_makejmp (u8_t * buff,u32_t size,long * ptr,u32_t inum) {
    static const u8_t ppj [] = {
        0x68,0x00,0x00,0x00,0x00,       /* pushl $0 */
        0x68,0x00,0x00,0x00,0x00,       /* pushl $0 */
        0x2E,0xFF,0x25,0x00,0x00,0x00,0x00   /* jmpl %cs:(0) */
    } ;
    memcpy (buff,ppj,sizeof (ppj)) ;        /* copy the code */
    memcpy (buff+6,&inum,sizeof (inum)) ;   /* patch the second push */
    memcpy (buff+13,&ptr,sizeof (ptr)) ;    /* patch the jump */
}

/*  *************************************************************
    Fixes interrupt entry points for interrupts with error code        
    *************************************************************   */
    
void i386_patchjmp (u8_t * buff,u32_t size) {
    memset (buff,0x90,5) ;
}

/*  *************************************************************
    Generates all interrupt entry points
    *************************************************************   */

void i386_alljmps (u8_t * buff,u32_t size,long * ptr) {
    uint i ;
    for (i = 0;(i < JUMPS) && (i*JMPSZ < size);++i)
        i386_makejmp (&buff [i*JMPSZ],JMPSZ,ptr,i) ;
    i386_patchjmp (&buff [8*JMPSZ],JMPSZ) ;
    i386_patchjmp (&buff [10*JMPSZ],JMPSZ) ;
    i386_patchjmp (&buff [11*JMPSZ],JMPSZ) ;
    i386_patchjmp (&buff [12*JMPSZ],JMPSZ) ;
    i386_patchjmp (&buff [13*JMPSZ],JMPSZ) ;
    i386_patchjmp (&buff [14*JMPSZ],JMPSZ) ;
    i386_patchjmp (&buff [17*JMPSZ],JMPSZ) ;
}

/*  *************************************************************
    Setup interrupt descriptor table
    *************************************************************   */

void i386_makeidt (idesc_t * idt,u32_t count,u8_t * jumps,u32_t size) {
    u32_t offset ;
    u16_t cs = i386_get_cs () ;
    uint i ;
    memset (idt,0,sizeof (*idt)*count) ;
    for (i = 0;i < count;++i) {
        offset = (u32_t)&jumps [size*i] ;
        idt [i].offset_lsw = offset & 0xffff ;
        idt [i].offset_msw = offset >> 16 ;
        idt [i].selector = cs ;
        idt [i].flags =  0x8E00 ; 
    }
    i386_lidt (idt,count*sizeof (*idt) - 1) ;
}

/*  *************************************************************
    Copy GDT
    *************************************************************   */

void i386_copygdt (gdesc_t * gdt,u32_t count) {
    treg_t treg ;

    i386_sgdt (&treg) ;
    memset (gdt,0,count * sizeof (*gdt)) ;
    memcpy (gdt,treg.base,treg.limit + 1) ;
    i386_lgdt (gdt,count * sizeof (*gdt) - 1) ;
    
}
/*  *************************************************************
    Create TSS descriptor
    *************************************************************   */

void i386_maketdesc (tdesc_t * desc,u32_t base,u32_t limit) {
    desc->limit0 = limit & 0xffff ;
    desc->limit1 = ((limit >> 16) & 0xf) |  0x90 ;
    desc->base0 = base ;
    desc->base1 = base >> 16 ;
    desc->base2 = base >> 24 ;
    desc->type = 0x89 ;
}

/*  *************************************************************
    Processor exception
    *************************************************************   */
	
void i386_exception (isr_t * isr) {

	static char const * const exstr [] = {
		"#DE - Divide error",
		"Unknown",
		"Non maskable interrupt",
		"#BP - Break point",
		"#OF - Overflow",
		"#BR - Bound range exceded",
		"#UD - Invald opcode",
		"#NM - Device not available",
		"#DF - Double fault",
		"Coporcessor segment overrun",
		"#TS - Invalid TSS",
		"#NP - Segment not pressent",
		"#SS - Stack fault",
		"#GP - General protection fault",
		"#PF - Page fault",
		"Unknown",
		"#MF - Floating-Point error",
		"#AC - Aligment check",
		"#MC - Machine check",
		"#XF - SIMD Floating-Point exception",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown"
	} ;

    u32_t a = 0x4 ;
    term_ioctl (TERM_SETATTR,&a) ;
    printf ("\tAn exception has occured at %x\t\t\t\t\t\n",isr->eip) ;
    printf ("\tCode %x (%s)\n",isr->inum,exstr [isr->inum & 0x1f]) ;
    printf ("\tRegisters :\n") ;
    printf ("\t\teax=%x ebx=%x ecx=%x edx=%x\n",isr->eax,isr->ebx,isr->ecx,isr->edx) ;
    printf ("\t\tedi=%x esi=%x ebp=%x esp=%x\n",isr->edi,isr->esi,isr->ebp,isr->old_esp) ;
	printf ("\t\t cs=%x  ds=%x  es=%x  ss=%x\n",isr->cs,isr->ds,isr->es,isr->old_ss) ;
    printf ("\t\t fs=%x  gs=%x\n",isr->fs,isr->gs) ;    
    printf ("\t\teflags=%x\n",isr->eflags) ;
    printf (
        "\t\tcr0=%x cr2=%x cr3=%x cr4=%x\n",
        i386_get_cr (0),
        i386_get_cr (2),
        i386_get_cr (3),
        i386_get_cr (4)
    ) ;
	i386_hlt () ;
}

/*  *************************************************************
    Interrupt callback 
    *************************************************************   */

void i386_isr (isr_t * isr) {
	u32_t irq = 0 ;
	if (isr->inum < 0x20) {
		i386_exception (isr) ;		
	}
	else if (isr->inum >= 0x20 && isr->inum < 0x30) {
		irq = isr->inum - 0x20 ;		
		switch (irq) {			
			case IRQ_TIMER:
				pit_isr () ;
				break ;				
			case IRQ_KEYBOARD:
				keyb_isr () ;
				break ;
			case IRQ_COM1OR3 :
				serial_isr (irq) ;
				break ;
			case IRQ_COM2OR4:
				serial_isr (irq) ;
				break ;
			case IRQ_RTC:
				rtc_isr () ;
				break ;
			default:
				break;
		}
		pic_eoi (irq) ;
	}
}

/*  *************************************************************
    Initialization code
    *************************************************************   */

void i386_init () {

    i386_alljmps (jumps,JUMPS*JMPSZ,&i386_isr_ptr) ;
    i386_makeidt (idt,JUMPS,jumps,JMPSZ) ;
    i386_copygdt (gdt,GDESCS) ;
    pic_remap_irq (0x20,0x28) ;	
}



