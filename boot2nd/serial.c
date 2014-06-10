#include <i386.h>
#include <serial.h>
#include <lib.h>
#include <pic.h>

#define SERIAL_COM1_MASK 0x01
#define SERIAL_COM2_MASK 0x02
#define SERIAL_COM3_MASK 0x04
#define SERIAL_COM4_MASK 0x08

#define RING_BUFFER_SIZE 2048

static u32_t G_ports_installed = 0 ;
static u08_t G_ring_buffer [4][RING_BUFFER_SIZE] ;

void serial_init () {
	int i ;
	for (i = 1;i <= 4;++i) {		
		if (!serial_probe_uart (i)) {		
			continue ;		
		}				
		serial_config (i,SERIAL_MAX_BAUD,SERIAL_DATA_BITS_8,SERIAL_PARITY_NONE,SERIAL_STOP_BITS_1,SERIAL_RX_IRQ) ;
		serial_port_enable (i) ;				
	}		
}


void serial_poll_port (int port) {
	register int c ;	
	while ((c = serial_rx_char (port)) > -1) {		
        putchar (c) ;
		serial_tx_char (port,c) ;
	}	
}

void serial_isr (int irq) {
	
	if (!G_ports_installed)
		return ;
	
	if (irq == IRQ_COM1OR3) {	
		if (G_ports_installed & SERIAL_COM1_MASK)
			serial_poll_port (1) ;
		if (G_ports_installed & SERIAL_COM3_MASK)
			serial_poll_port (3) ;
	}
	else if (irq == IRQ_COM2OR4) {
		if (G_ports_installed & SERIAL_COM1_MASK)
			serial_poll_port (2) ;
		if (G_ports_installed & SERIAL_COM3_MASK)
			serial_poll_port (4) ;	
	}	
}

u16_t serial_get_port_address (int port) {
	switch (port) {
		case SERIAL_COM1 : 
			return SERIAL_COM1_ADDRESS ;
		case SERIAL_COM2 : 
			return SERIAL_COM2_ADDRESS ;
		case SERIAL_COM3 : 
			return SERIAL_COM3_ADDRESS ;
		case SERIAL_COM4 : 
			return SERIAL_COM4_ADDRESS ;			
	}
	return serial_get_port_address (SERIAL_COM1) ;
}

void serial_config (int port,u32_t baud_rate,u32_t data,u32_t parity,u32_t stop,u32_t interrupt_enable) {
	register u16_t address = serial_get_port_address (port) ;
	register u16_t divisor = SERIAL_MAX_BAUD / baud_rate ;
	
	outb (address+1,0x00) ;    
	outb (address+3,SERIAL_DLAB_MODE) ;    
	outb (address+0,divisor & 0xff) ;    
	outb (address+1,divisor >> 8) ;    
	outb (address+3,((parity & 3) << 3) | ((stop & 1) << 2) | ((data - 5) & 3)) ;    
	outb (address+2,0xC7) ;    
	outb (address+4,0x0B) ;
	outb (address+1,interrupt_enable&0xff) ;
}

int serial_rx_ready (int port) {
	register u16_t address = serial_get_port_address (port) ;
	return inb (address+5) & 0x01 ;
}

int serial_tx_ready (int port) {
	register u16_t address = serial_get_port_address (port) ;
	return inb (address+5) & 0x20 ;
}

int serial_rx_char (int port) {	
	register u16_t address = serial_get_port_address (port) ;
	if (inb (address+5) & 0x01) {
		return inb (address) ;
	}
	return -1 ;
}

int serial_tx_char (int port,int data) {
	register u16_t address = serial_get_port_address (port) ;
	if (inb (address+5) & 0x20) {
		outb (address,data) ;
		return !0 ;
	}
	return 0 ;
}

void serial_port_enable (int port) {
	G_ports_installed |= (1 << (port-1)) ;
}

void serial_port_disable (int port) {
	G_ports_installed &= (~(1 << (port-1))) ;
}

int serial_is_port_enabled (int port) {
	return G_ports_installed & (1 << (port-1)) ? 1 : 0 ;
}

int serial_probe_uart (int port) {
	
	u16_t baseaddr = serial_get_port_address (port) ;
	int x,olddata ;	
	
	olddata = inpb (baseaddr+4) ;
	outpb (baseaddr+4,0x10) ;
	if (inpb (baseaddr+6) & 0xf0) 
		return SERIAL_UART_NONE ;
		
	outpb (baseaddr+4,0x1f) ;
	if ((inpb(baseaddr+6)&0xf0)!=0xf0) 
		return SERIAL_UART_NONE ;		
		
	outpb (baseaddr+4,olddata) ;
	olddata = inpb (baseaddr+7) ;
	outpb (baseaddr+7,0x55) ;
	if (inpb(baseaddr+7)!=0x55) 
		return SERIAL_UART_8250 ;
		
	outpb (baseaddr+7,0xAA) ;
	if (inpb(baseaddr+7)!=0xAA) 
		return SERIAL_UART_8250 ;
		
	outpb (baseaddr+7,olddata) ; 
	outpb (baseaddr+2,1) ;
	x = inpb (baseaddr+2) ;
	outpb (baseaddr+2,0x0) ;
	
	if (!(x&0x80)) 
		return SERIAL_UART_16450 ;
	if (!(x&0x40)) 
		return SERIAL_UART_16550 ;
		
	return SERIAL_UART_16550A ;
}

void serial_tx_buffer (int port,void const * _buffer,u32_t len) {
	register char const * buffer = (char const *)_buffer ;
	int i = 0;
	for (i = 0;i < len;++i) {
		while (!serial_tx_char (port,buffer [i])) ;		
	}
}

void serial_tx_string (int port,char const * buffer) {
	int i = 0;
	while (buffer [i]) {
		while (!serial_tx_char (port,buffer [i])) ;
		++i ;
	}
}




