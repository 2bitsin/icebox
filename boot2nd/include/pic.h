#ifndef __ARCH_X86_I8259A_H__
#define __ARCH_X86_I8259A_H__

/*  remaps the master and slave IRQ contoler to specified values */
void pic_reamap_irq (u32_t master,u32_t slave) ;

/*  turns an interrupt on or off */
void pic_mask_irq (u32_t interrupt,u32_t state) ;

void pic_master_ack () ;
void pic_slave_ack () ;
void pic_eoi (u32_t irq) ;

#define IRQ_OFF         (0)
#define IRQ_ON          (!IRQ_OFF) 

#define IRQ_TIMER       0
#define IRQ_KEYBOARD    1
#define IRQ_CASCADE		2
#define IRQ_COM2OR4 	3
#define IRQ_COM1OR3 	4	
#define IRQ_LPT2		5
#define IRQ_FDC			6
#define IRQ_LPT1		7
#define IRQ_RTC			8
#define IRQ_PS2_MOUSE	12
#define IRQ_FPU			13
#define IRQ_ATA1		14
#define IRQ_ATA2		15


#endif /* __ARCH_X86_I8259A_H__ */
