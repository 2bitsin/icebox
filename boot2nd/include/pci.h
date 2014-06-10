#ifndef __PCI_H__
#define __PCI_H__


#define PCI_BUS_MASK 					0xff
#define PCI_DEVICE_MASK 				0x1f
#define PCI_FUNCTION_MASK 				0x7
#define PCI_REGISTER_MASK 				0xfc

#define PCI_NO_DEVICE					0xffffffff
#define PCI_INVALID_ID					0xffff

typedef 
	struct {
		u16_t vendor_id ;
		u16_t device_id ;
		u16_t command ;
		u16_t status ;
		u08_t revision ;
		u08_t progif ;
		u08_t sub_class ;
		u08_t dev_class ;
		u08_t cache_line_size ;
		u08_t latency_timer ;
		u08_t header_type ;
		u08_t bist ;			
		union {
			struct {
				u32_t bar0 ;
				u32_t bar1 ;
				u32_t bar2 ;
				u32_t bar3 ;
				u32_t bar4 ;
				u32_t bar5 ;
				u32_t cardbus_cis_pointer ;
				u16_t subsystem_vendor_id ;
				u16_t subsystem_id ;
				u32_t erom_base_address ;
				u32_t capabilities_pointer ;
				u32_t reserved ;
				u08_t interrupt_line ;
				u08_t interrupt_pin ;
				u08_t min_grant ;
				u08_t max_latency ;				
			}
			device ;
			struct {
				u32_t bar0 ;
				u32_t bar1 ;
				u08_t primary_bus_number ;
				u08_t secondary_bus_number ;
				u08_t subordinate_bus_number ;
				u08_t secondary_latency_timer ;
				u16_t memory_base ;
				u16_t prefetchable_memory_limit ;
				u32_t prefetchable_base ;
				u32_t prefetchable_limit ;
				u16_t io_base ;
				u16_t io_limit ;
				u32_t capabilities_pointer ;
				u32_t erom_base_address ;
				u08_t interrupt_line ;
				u08_t interrupt_pin ;
				u16_t bridge_control ;				
			}
			pci_to_pci_bridge ;
			struct {
				u32_t cardbus_base_address ;
				u16_t offset_of_capability_list ;
				u16_t secondary_status ;
				u08_t pci_bus_number ;
				u08_t cardbus_number ;
				u08_t subordinate_bus_number ;
				u08_t cardbus_latency_timer ;
				u32_t memory_base_0 ;
				u32_t memory_limit_0 ;
				u32_t memory_base_1 ;
				u32_t memory_limit_1 ;
				u32_t io_base_0 ;
				u32_t io_limit_0 ;
				u32_t io_base_1 ;
				u32_t io_limit_1 ;
				u08_t interrupt_line ;
				u08_t interrupt_pin ;
				u16_t bridge_control ;			
				// ==============================
				u16_t subsystem_vendor_id ;
				u16_t subsystem_id ;
				u32_t legacy_mode_base_address ;
			} 
			pci_to_cardbus_bridge ;
		} ;		
	}
	pci_device_data_t ;

#define PCI_HEADER_STANDARD_DEVICE		0x00
#define PCI_HEADER_PCI_TO_PCI_BRIDGE 	0x01
#define PCI_HEADER_CARDBUS_BRDIGE		0x02
#define PCI_HEADER_TYPE_MASK			0x7f
#define PCI_HEADER_MULTIFUNCTIONAL		0x80

#define PCI_COMMAND_IO_SPACE			0x0001
#define	PCI_COMMAND_MEMORY_SPACE		0x0002
#define PCI_COMMAND_BUS_MASTER			0x0004
#define PCI_COMMAND_SPECIAL_CYCLES		0x0008
#define PCI_COMMAND_MEM_WR_INVD			0x0010
#define PCI_COMMAND_VGA_PALETTE_SNOOP	0x0020
#define PCI_COMMAND_PARITY_ERROR		0x0040
#define PCI_COMMAND_SERR_ENABLE			0x0100
#define PCI_COMMAND_FAST_BTB_ENABLE		0x0200
#define PCI_COMMAND_INTERRUPT_DISABLE	0x0400
	
#define PCI_BIST_COMPLETION_CODE_MASK	0x0f
#define PCI_BIST_CAPABLE				0x80
#define PCI_BIST_START					0x40

#define PCI_CLASS_STORAGE				0x01
#define PCI_SUBCLASS_IDE				0x01

void pic_init () ;
u32_t pci_read_long (u8_t bus,u8_t dev,u8_t func,u8_t reg) ;
u16_t pci_read_word (u8_t bus,u8_t dev,u8_t func,u8_t reg) ;

u16_t pci_get_vendor_id (u8_t bus,u8_t dev,u8_t fun) ;
u16_t pci_get_device_id (u8_t bus,u8_t dev,u8_t fun) ;
u8_t  pci_get_header_type (u8_t bus,u8_t dev,u8_t fun) ;
char const * pci_get_dev_type_name (u08_t devclass,u08_t subclass,u08_t progif) ;
pci_device_data_t * pci_read_device_data (pci_device_data_t * data,u8_t bus,u8_t dev,u8_t fun) ;

	
#endif


