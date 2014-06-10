#ifndef __CONFIG_H__
#define __CONFIG_H__

#define _1KB						0x400
#define _1MB 						0x100000
#define _1GB						0x40000000

#define MAP_ATTR_IGNORE				0x01
#define MAP_ATTR_NON_VOLATILE		0x02

#define MEMORY_REQUIRED 			64
#define HEAP_LIMIT					8
#define HEAP_ALIGMENT				16

#define ADDR_PARAM_BASE				0x7dfc 
#define	ADDR_PARAM_CPU				ADDR_PARAM_BASE+0
#define	ADDR_PARAM_MEM 				ADDR_PARAM_BASE+4
#define	ADDR_PARAM_MMAP_SIZE 		ADDR_PARAM_BASE+8
#define	ADDR_PARAM_MMAP 			ADDR_PARAM_BASE+12

#define MAP_TYPE_NORMAL				1
#define	MAP_TYPE_RESERVED			2
#define MAP_TYPE_ACPI_RECLAIMABLE	3
#define MAP_TYPE_ACPI_NVS_MEMORY	4
#define MAP_TYPE_BAD_MEMORY			5

#define IDE_MAX_CONTROLLERS			4

typedef
	struct {
		u64_t base ;
		u64_t limit ;
		u32_t type ;
		u32_t attr ;
	} 
	smap_t ;

typedef 
	struct {
		u32_t cpu ;
		u32_t mem ;
		u32_t entries ;
		smap_t mmap [] ;
	} 
	params_t ;
	
void config_init () ;
	
#endif
	
