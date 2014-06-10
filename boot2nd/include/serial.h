#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_COM1_ADDRESS 	0x3F8
#define SERIAL_COM2_ADDRESS 	0x2F8
#define SERIAL_COM3_ADDRESS 	0x3E8
#define SERIAL_COM4_ADDRESS 	0x2E8

#define SERIAL_COM1				1
#define SERIAL_COM2				2
#define SERIAL_COM3				3
#define SERIAL_COM4				4

#define SERIAL_PARITY_NONE		0
#define SERIAL_PARITY_ODD		1
#define SERIAL_PARITY_EVEN		3
#define SERIAL_PARITY_MARK		5
#define SERIAL_PARITY_SPACE		7

#define SERIAL_STOP_BITS_1		0
#define SERIAL_STOP_BITS_1_5	1
#define SERIAL_STOP_BITS_2		1

#define SERIAL_DATA_BITS_5		5
#define SERIAL_DATA_BITS_6		6
#define SERIAL_DATA_BITS_7		7
#define SERIAL_DATA_BITS_8		8

#define SERIAL_MAX_BAUD			115200 
#define SERIAL_DLAB_MODE		0x80

#define SERIAL_UART_NONE		0
#define SERIAL_UART_8250		1
#define SERIAL_UART_16450		2
#define SERIAL_UART_16550		3
#define SERIAL_UART_16550A		4

#define SERIAL_RX_IRQ			1

	void 	serial_init () ;
	int 	serial_probe_uart (int port) ;
	void 	serial_isr (int irq) ;
	u16_t 	serial_get_port_address (int port) ;
	void 	serial_config (int port,u32_t baud_rate,u32_t data,u32_t parity,u32_t stop,u32_t interrupt_enable) ;
	int 	serial_rx_ready (int port) ;
	int 	serial_tx_ready (int port) ;
	int 	serial_rx_char (int port) ;
	int 	serial_tx_char (int port,int data) ;
	void 	serial_port_enable (int port) ;
	void 	serial_port_disable (int port) ;
	int 	serial_is_port_enabled (int port) ;
	void 	serial_tx_buffer (int port,void const * buffer,u32_t len) ;
	void 	serial_tx_string (int port,char const * buffer) ;

#endif

