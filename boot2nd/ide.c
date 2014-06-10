#include <types.h>
#include <pci.h>
#include <ide.h>
#include <assert.h>
#include <config.h>
#include <lib.h>
#include <i386.h>

void ide_write (u08_t channel,u08_t reg,u08_t data) ;
u08_t ide_read (u08_t channel,u08_t reg) ;
	
typedef 	
	struct  {
		u08_t reserved ;     // 0 (Empty) or 1 (This Drive really exists).
		u08_t channel ;      // 0 (Primary Channel) or 1 (Secondary Channel).
		u08_t drive ;        // 0 (Master Drive) or 1 (Slave Drive).
		u16_t type ;         // 0: ATA, 1:ATAPI.
		u16_t signature ;    // Drive Signature
		u16_t capabilities;  // Features.
		u32_t command_sets;  // Command Sets Supported.
		u32_t size ;         // Size in Sectors.
		s08_t model [41] ;   // Model in string.
	}
	ide_device_data_t ;

typedef 
	struct {
		u08_t bus,dev,fun ;
		pci_device_data_t pci_data ;
		struct {
			u32_t base_port ;
			u32_t control_port ;			
			u32_t bmi_base_port ;
			u08_t irq_enabled ;
		}
		channels [2] ;
		ide_device_data_t devices [4] ;
	}
	ide_channel_data_t ;
	

static u32_t g_ide_index = 0 ;
static ide_channel_data_t g_ide [IDE_MAX_CONTROLLERS] ;


void ide_init (pci_device_data_t const * data,u08_t bus,u08_t dev,u08_t fun) {

	ide_channel_data_t * channel = &g_ide [g_ide_index++] ;
	assert (g_ide_index < IDE_MAX_CONTROLLERS) ;
	
	channel->bus = bus ;
	channel->dev = dev ;
	channel->fun = fun ;
	
	memcpy (&channel->pci_data,data,sizeof (*data)) ;
	
	channel->channels [ATA_PRIMARY].bmi_base_port = 
		channel->pci_data.device.bar5 + 0 ;
		
	channel->channels [ATA_SECONDARY].bmi_base_port = 
		channel->pci_data.device.bar5 + 8 ;

	channel->channels [ATA_PRIMARY].base_port = 
		channel->pci_data.device.bar0 ? 
			channel->pci_data.device.bar0 & ~3L : 
			ATA_PRIMARY_DEFAULT_BASE ;
			
	channel->channels [ATA_PRIMARY].control_port = 	
		channel->pci_data.device.bar1 ? 
			channel->pci_data.device.bar1 & ~3L : 
			ATA_PRIMARY_DEFAULT_CONTROL ;
			
	channel->channels [ATA_SECONDARY].base_port = 
		channel->pci_data.device.bar2 ? 
			channel->pci_data.device.bar2 & ~3L : 
			ATA_SECONDARY_DEFAULT_BASE ;
			
	channel->channels [ATA_SECONDARY].control_port = 
		channel->pci_data.device.bar3 ? 
			channel->pci_data.device.bar3 & ~3L : 
			ATA_SECONDARY_DEFAULT_CONTROL ;
			
	ide_write (ATA_PRIMARY,ATA_REG_CONTROL,2) ;
	ide_write (ATA_SECONDARY,ATA_REG_CONTROL,2) ;
	
}

void ide_write (u08_t channel,u08_t reg,u08_t data) {
	ide_channel_data_t * device = &g_ide [0] ;
	
	if (reg > 0x07 && reg < 0x0C) 
		ide_write (channel,ATA_REG_CONTROL,0x80|device->channels [channel].irq_enabled) ;
		
	if (reg < 0x08) 
		outb (data,device->channels [channel].base_port + reg - 0x00) ;		
	else if (reg < 0x0C) 
		outb (data,device->channels [channel].base_port + reg - 0x06) ;				
	else if (reg < 0x0E) 
		outb (data,device->channels [channel].control_port + reg - 0x0A) ;		
	else if (reg < 0x16) 
		outb (data,device->channels [channel].bmi_base_port + reg - 0x0E) ;
		
	if (reg > 0x07 && reg < 0x0C) 
		ide_write (channel,ATA_REG_CONTROL,device->channels [channel].irq_enabled) ;
}

u08_t ide_read (u08_t channel,u08_t reg) {
	ide_channel_data_t * device = &g_ide [0] ;
	
	u08_t result ;
	if (reg > 0x07 && reg < 0x0C)
		ide_write (channel,ATA_REG_CONTROL,0x80|device->channels [channel].irq_enabled) ;
		
	if (reg < 0x08)
		result = inb (device->channels [channel].base_port + reg - 0x00) ;
	else if (reg < 0x0C)
		result = inb (device->channels [channel].base_port  + reg - 0x06) ;
	else if (reg < 0x0E)
		result = inb (device->channels [channel].control_port  + reg - 0x0A) ;
	else if (reg < 0x16)
		result = inb (device->channels [channel].bmi_base_port + reg - 0x0E) ;
		
	if (reg > 0x07 && reg < 0x0C)
		ide_write (channel,ATA_REG_CONTROL,device->channels [channel].irq_enabled) ;
   return result ;
}


