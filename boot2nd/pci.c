#include <types.h>
#include <pci.h>
#include <i386.h>
#include <lib.h>
#include <ide.h>

#define CONFIG_ADDRESS	0xCF8
#define CONFIG_DATA 	0xCFC

static inline u32_t pci_make_address (u8_t bus,u8_t dev,u8_t func,u8_t reg) {
	return 0x80000000L | 
		((bus & PCI_BUS_MASK) << 16) | 
		((dev & PCI_DEVICE_MASK) << 11) | 
		((func & PCI_FUNCTION_MASK) << 8) | 
		(reg & PCI_REGISTER_MASK) ;
}

u32_t pci_read_long (u8_t bus,u8_t dev,u8_t func,u8_t reg) {
	outpl (CONFIG_ADDRESS,
		pci_make_address (bus,dev,func,reg)) ;
	inpl (CONFIG_DATA) ;
}

u16_t pci_read_word (u8_t bus,u8_t dev,u8_t func,u8_t reg) {
	return pci_read_long (bus,dev,func,reg) >> ((reg & 2) * 8) ;
}

u16_t pci_get_device_id (u8_t bus,u8_t dev,u8_t fun) {	
	return pci_read_word (bus,dev,fun,0) ;
}

u16_t pci_get_vendor_id (u8_t bus,u8_t dev,u8_t fun) {	
	return pci_read_word (bus,dev,fun,0+2) ;
}

u8_t pci_get_header_type (u8_t bus,u8_t dev,u8_t fun) {
	return pci_read_word (bus,dev,fun,0xc+2) ;
}

pci_device_data_t * pci_read_device_data (pci_device_data_t * data,u8_t bus,u8_t dev,u8_t fun) {
	u32_t * _data = (u32_t *)data ;
	int i ;
	for (i = 0;i < 18;++i) 
		_data [i] = pci_read_long (bus,dev,fun,i*4) ;	
	return data ;
}

void pci_register_device (pci_device_data_t const * data,u08_t bus,u08_t dev,u08_t fun) {
	if (data->dev_class == PCI_CLASS_STORAGE && 
		data->sub_class == PCI_SUBCLASS_IDE) 
	{
		ide_init (data,bus,dev,fun) ;
		return ;
	}	
}

void pci_enumerate () {
	pci_device_data_t data ;
	u32_t i,bus,dev,fun ;
	for (i = 0;i < 256*32*8;++i) {		
		bus = (i >> 8) & 0xff ;
		dev = (i >> 3) & 0x1f ;
		fun = (i & 0x7) ;
		if (pci_get_vendor_id (bus,dev,fun) == PCI_INVALID_ID)
			continue ;
		pci_read_device_data (&data,bus,dev,fun) ;		
		pci_register_device (&data,bus,dev,fun) ;		
	}
}

char const * pci_get_dev_type_name (u08_t devclass,u08_t subclass,u08_t progif) {
	#if 1
	switch (devclass) {
		case 0x00 : 
			switch (subclass) {
				case 0x0: return "Unspecified Non-VGA" ; 
				case 0x1: return "Unspecified VGA-Compatible" ;
				default: return "Unspecified" ;
			}			
		case 0x01 :
			switch (subclass) {
				case 0x00 : return "SCSI Bus Controller" ;
				case 0x01 : return "IDE Controller" ;
				case 0x02 : return "Floppy Disk Controller" ;
				case 0x03 : return "IPI Bus Controller" ;
				case 0x04 : return "RAID Controller" ;
				case 0x05 : 
					switch (progif) {
						case 0x20 : return "ATA Controller (Single DMA)" ; 
						case 0x30 : return "ATA Controller (Chained DMA)" ;
						default : return "ATA Controller" ;
					} ;
				case 0x06 : 
					switch (progif) {
						case 0x0 : return "Serial ATA (Vendor Specific Interface)" ;
						case 0x1 : return "Serial ATA (AHCI 1.0)" ;
						default : return "Serial ATA" ;
					}
				case 0x07 : return "Serial Attached SCSI" ;
				default : return "Mass Storage Controller" ;				
			} ;
		case 0x02 :
			switch (subclass) {
				case 0x00 : return "Ethernet Controller" ;
				case 0x01 : return "Token Ring Controller" ;
				case 0x02 : return "FDDI Controller" ;
				case 0x03 : return "ATM Controller" ;
				case 0x04 : return "ISDN Controller" ;
				case 0x05 : return "WorldFip Controller" ;
				case 0x06 : return "PICMG 2.14 Multi Computing" ;
				default : return "Network controller" ;
			} ;
		case 0x03 :
			switch (subclass) {
				case 0x00 :
					switch (progif) {
						case 0x00 : return "VGA-Compatible Controller" ;
						case 0x01 : return "8512-Compatible Controller" ;
						default : "VGA Controller" ;
					} ;					
				case 0x01 : return "XGA Controller" ;
				case 0x02 : return "3D Controller (Not VGA-Compatible)" ;
				default : return "Display Controller" ;
			} ;
		case 0x04 :
			switch (subclass) {
				case 0x00 : return "Video Device" ;
				case 0x01 : return "Audio Device" ;
				case 0x02 : return "Computer Telephony Device" ;
				default: "Multimedia Device" ;
			} ;
		case 0x05 :
			switch (subclass) {
				case 0x00 : return "RAM Controller" ;
				case 0x01 : return "Flash Controller" ;
				default : return "Memory Controller" ;
			} ;
		case 0x06 :
			switch (subclass) {
				case 0x00 : return "Host Bridge" ;
				case 0x01 : return "ISA Bridge" ;
				case 0x02 : return "EISA Bridge" ;
				case 0x03 : return "MCA Bridge" ;
				case 0x04 : return "PCI-PCI Bridge" ;
				case 0x05 : return "PCMCIA Bridge" ;
				case 0x06 : return "NuBus Bridge" ;
				case 0x07 : return "CardBus Bridge" ;
				case 0x08 : return "RACEway Bridge" ;
				case 0x09 : return "PCI-PCI Bridge (Semi-Transparent)" ;
				case 0x0a : return "InfiniBand-PCI Host Bridge" ;
				default : return "Bridge Device" ;
			} ;
		case 0x07 :
			switch (subclass) {
				case 0x00 : 
					switch (progif) {
						case 0x00 : return "Generic XT-Compatible Serial Controller" ;
						case 0x01 : return "16450-Compatible Serial Controller" ;
						case 0x02 : return "16550-Compatible Serial Controller" ;
						case 0x03 : return "16650-Compatible Serial Controller" ;
						case 0x04 : return "16750-Compatible Serial Controller" ;
						case 0x05 : return "16850-Compatible Serial Controller" ;
						case 0x06 : return "16950-Compatible Serial Controller" ;
						default : return "Serial Controller" ;
					} ;
				case 0x01 :
					switch (progif) {
						case 0x00 : return "Parallel Port" ;
						case 0x01 : return "Bi-Directional Parallel Port" ;
						case 0x02 : return "ECP 1.X Compliant Parallel Port" ;
						case 0x03 : return "IEEE 1284 Controller" ;
						case 0xFE : return "IEEE 1284 Target Device" ;
						default : return "Parallel Port" ;
					} ;
				case 0x02 : return "Multiport Serial Controller" ;
				case 0x03 : 
					switch (progif) {
						case 0x00 : return "Generic Modem" ;
						case 0x01 : return "Hayes Compatible Modem (16450-Compatible Interface)" ;
						case 0x02 : return "Hayes Compatible Modem (16550-Compatible Interface)" ;
						case 0x03 : return "Hayes Compatible Modem (16650-Compatible Interface)" ;
						case 0x04 : return "Hayes Compatible Modem (16750-Compatible Interface)" ;
						default: "Generic Modem" ;
					} ;
				case 0x04 : return "IEEE 488.1/2 (GPIB) Controller" ;
				case 0x05 : return "Smart Card" ;
				default : return "Communications Device" ;
			} ;
		case 0x0C :
			switch (subclass) {
				case 0x00 : 
					switch (progif) { 
						case 0x00 : return "IEEE 1394 Controller (FireWire)" ;
						case 0x10 : return "IEEE 1394 Controller (OpenHCI)" ;
						default : return "IEEE 1394 Controller" ;
					} ;
				case 0x01 : return "ACCESS Bus" ;
				case 0x02 : return "SSA" ;
				case 0x03 : 
					switch (progif) {
						case 0x00 : return "USB (UHCI)" ;
						case 0x10 : return "USB (OHCI)" ;
						case 0x20 : return "USB (EHCI)" ;
						case 0xFE : return "USB (Not Host Controller)" ;
						default : return "USB" ;						
					} ;
				case 0x04 : return "Fibre Channel" ;
				case 0x05 : return "SMBus" ;
				case 0x06 : return "InfiniBand" ;
				case 0x07 : 
					switch (progif) {
						case 0x00 : return "IPMI SMIC Interface" ;
						case 0x01 : return "IPMI Keyboard controller" ;
						case 0x02 : return "IPMI Block transfer interface" ;
						default : return "IPMI" ;
					} ;
				case 0x08 : return "SERCOS Interface Standard (IEC 61491)" ;
				case 0x09 : return "CANBus" ;
				default : "Serial Bus Controller" ;
			} ;
		
	} ;
	#endif
	return "Unknown" ;
}

void pci_init () {
	pci_enumerate () ;
}


