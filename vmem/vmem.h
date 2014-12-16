#ifndef __VMEMH__
#define __VMEMH__

#include "../types.h"

#define NULL					0

// Translation
#define ADDRESS_OFFSET			4096

#define PAGE_SIZE 				4096
#define SECON_LVL_TT_COUN		256
#define SECON_LVL_TT_SIZE		4*SECON_LVL_TT_COUN
#define FIRST_LVL_TT_COUN		4096
#define FIRST_LVL_TT_SIZE		4*FIRST_LVL_TT_COUN
#define TOTAL_TT_SIZE			(FIRST_LVL_TT_SIZE + SECON_LVL_TT_SIZE*FIRST_LVL_TT_COUN)

#define FIRST_LVL_ADDR			0x48000
#define SECON_LVL_ADDR			FIRST_LVL_ADDR + FIRST_LVL_TT_SIZE

#define MASK12					4095
#define FIRST20(x)				(((uint32_t)x)&(~4095))
#define FIRST22(x)				(((uint32_t)x)&(~1023))
/*
#define DEVICE_ADDR_BEGIN		0x20000000
#define DEVICE_ADDR_END			0x20FFFFFF

#define USER_SPACE_BEGIN		0x500000

#define MASK12					4095
#define FIRST20(x)				(x&(~4095))
#define FIRST22(x)				(x&(~1023))

#define FIRST_INDEX(x)			(x>>20)
#define SECOND_INDEX(x)			((x>>12)&255)
#define PAGE_INDEX(x)			(x&4095)

#define SECOND_DESCRIPTOR(add)	(SECON_LVL_ADDR + 4*FIRST_INDEX(add)*SECON_LVL_TT_SIZE + 4*SECOND_INDEX(add))
#define FIRST_DESCRIPTOR(add)	(FIRST_LVL_ADDR + 4*FIRST_INDEX(add))

// Free pages
#define FREE_PAGES_ADDR			FIRST_LVL_ADDR + TOTAL_TT_SIZE
#define FREE_PAGES_COUN			1048576
#define FREE_PAGES_SIZE			FREE_PAGES_COUN
*/
typedef uint32 	uint32_t;
typedef uint8 	uint8_t;

void init_no_translation_pages(uint32_t* first_table);

int init_kern_translation_table(void);

void start_mmu_C(void);

void configure_mmu_C(void);

uint8_t* vMem_Alloc( unsigned int nbPages );

void vMem_Free( uint8_t* ptr, unsigned int nbPages );

uint32_t* new_first_table();
uint32_t* new_second_table();

#endif