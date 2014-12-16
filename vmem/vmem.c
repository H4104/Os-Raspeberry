#include "vmem.h"

uint32_t total;


#define MINI_TABLE_ADDR			0x48000
#define MINI_TABLE_COUN			0x2000
#define	MINI_TABLE_GET_ADDR(n)	(TT_START_ADDR + 1024*n)
#define NO_MORE_SPACE			0

#define OCCP_TABLE_ADDR			0x50000
#define OCCP_TABLE_COUN			0x100000 // 1 mo or 0x20000 if /8

#define TT_START_ADDR			0x150000
#define TT_MAX_ADDR				0x500000


static uint32_t device_flags = 1079; 	//010000110111
static uint32_t kernel_flags = 1138; 	//010001110010
//static uint32_t* first_level_entries;

static uint32_t	t_table;
static uint8_t* occupation_table;
static uint8_t* mini_table;

static uint32_t* kernel_tt_address;

int init_kern_translation_table(void)
{
	int i;

	mini_table = (uint8_t*)MINI_TABLE_ADDR;
	
	for( i = 0; i < MINI_TABLE_COUN; ++i){
		mini_table[i] = 0;
	}

	occupation_table =(uint8_t*)OCCP_TABLE_ADDR;

	for( i = 0; i < OCCP_TABLE_COUN; ++i){
		occupation_table[i] = 0;
	}

	kernel_tt_address = new_first_table();
	init_no_translation_pages(kernel_tt_address);

	// todo : init_no_translation_pages
	return 0;
}

void init_no_translation_pages(uint32_t* first_table)
// adresses 0x0 to 0x50000 = 5mega
// A first level descriptor can handle 4096*256 = 1mega = 1048576 adresses 
// so we need to fill 5 descriptors
{
	int i;
	for( i = 0; i < 5; ++i)
	{
		uint32_t* second_table = new_second_table();
		first_table[i] = FIRST22(second_table) | 1;

		int j;
		for( j = 0; j < SECON_LVL_TT_COUN; ++j)
		{
			second_table[j] = FIRST20(i*0x100000+j*PAGE_SIZE) | kernel_flags;
		}
	}
	for( i = 0; i < 16; ++i) // 16*0x100 000 to get 0x1 000 000
	{
		uint32_t* second_table = new_second_table();
		first_table[i+5] = FIRST22(second_table) | 1;
		int j;
		for( j = 0; j < SECON_LVL_TT_COUN; ++j)
		{
			second_table[j] = FIRST20(0x20000000+i*0x100000+j*PAGE_SIZE) | device_flags;
		}
	}
}

uint32_t* new_first_table()
{
	int i;
	int j;
	for( i = 0; i < MINI_TABLE_COUN; ++i)
	{
		for(j = i; j < i+16; ++j)
		{
			if(mini_table[j] != 0)
			{
				i = j+1;
				break;
			}
			if( j == i+15 )
			{
				int k;
				for( k = 0; k < 16; ++k)
				{
					mini_table[i+k] = 1;
				}
				return (uint32_t*)MINI_TABLE_GET_ADDR(i);
			}
		}
		
	}
	// If we are here, then the current process should be shut down
	return NO_MORE_SPACE;
}

uint32_t* new_second_table()
{
	int i;
	for( i = 0; i < MINI_TABLE_COUN; ++i)
	{
		if(mini_table[i] == 0)
		{
			mini_table[i] = 1;
			return (uint32_t*)MINI_TABLE_GET_ADDR(i);
		}
	}
	// If we are here, then the current process should be shut down
	return NO_MORE_SPACE;
}

/*static void init_page_no_translation(uint32_t address, uint32_t sec_flag)
{
	uint32_t* first_level_descriptor = (uint32_t*)FIRST_DESCRIPTOR(address);
	if( (*first_level_descriptor & 3) == 0 )
	{
		// Ignored
		*first_level_descriptor =  FIRST22(SECOND_DESCRIPTOR(address)) | 1;
		
		uint32_t* second_level_descriptor = (uint32_t*)SECOND_DESCRIPTOR(address);

		uint32_t i;
		for( i = 0; i < SECON_LVL_TT_COUN; ++i){
			second_level_descriptor[i] = 0;
		}
	}
	uint32_t* second_level_descriptor = (uint32_t*)SECOND_DESCRIPTOR(address);

	if( (*second_level_descriptor) == 0 )
	{
		*second_level_descriptor = FIRST20(address) | sec_flag;
	}
	
}

int init_kern_translation_table(void)
{
	first_level_entries = (uint32_t*)FIRST_LVL_ADDR;

	// On met toutes les entrées à Translation Fault
	int i;
	for( i = 0; i < FIRST_LVL_TT_COUN; ++i)
	{
		first_level_entries[i] = 0; // Deux derniers bits à 0
	}

	uint32_t address;
	for( address = 0; address < USER_SPACE_BEGIN; address += ADDRESS_OFFSET)
	{
		init_page_no_translation(address, kernel_flags);
	}
	for( address = DEVICE_ADDR_BEGIN; address < DEVICE_ADDR_END; address += ADDRESS_OFFSET)
	{
		init_page_no_translation(address, device_flags);
	}


	return 0;
}
*/
void start_mmu_C(void)
{
	register unsigned int control;
	__asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0)); //Disable cache
	__asm("mcr p15, 0, r0, c7, c7, 0"); //Invalidate cache (data and instructions)
	__asm("mcr p15, 0, r0, c8, c7, 0"); //Invalidate TLB entries
	// Enable ARMv6 MMU features (disable sub-page AP) 
	control = (1<<23) | (1 << 15) | (1 << 4) | 1;
	// Invalidate the translation lookaside buffer (TLB) 
	__asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
	// Write control register 
	__asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

}

void configure_mmu_C(void)
{
	register unsigned int pt_addr = FIRST_LVL_ADDR;
	total++;
	// Translation table 0 
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
	// Translation table 1 
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
	// Use translation table 0 for everything 
	__asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
	// Set Domain 0 ACL to "Manager", not enforcing memory permissions
	// Every mapped section/page is in domain 0
	//
	__asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}

uint8_t* vMem_Alloc( unsigned int nbPages )
{
	return NULL;
}

void vMem_Free( uint8_t* ptr, unsigned int nbPages )
{

}
