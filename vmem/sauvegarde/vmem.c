#include "vmem.h"

uint32_t total;

static uint32_t device_flags = 1079; 	//010000110111
static uint32_t kernel_flags = 1138; 	//010001110010
static uint32_t* first_level_entries;

static uint8_t* pages_occupation;

static void init_page_no_translation(uint32_t address, uint32_t sec_flag)
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

	// initializing occupation table
	pages_occupation = (uint8_t*)FREE_PAGES_ADDR;

	/*for( i = 0; i < USER_SPACE_BEGIN / PAGE_SIZE ; ++i)
	{
		pages_occupation[i] = 0;
	}
	for( i = USER_SPACE_BEGIN / PAGE_SIZE ; i < DEVICE_ADDR_BEGIN / PAGE_SIZE ; ++i)
	{
		pages_occupation[i] = 0;
	}
	for( i = DEVICE_ADDR_BEGIN / PAGE_SIZE; i < FREE_PAGES_COUN; ++i)
	{
		pages_occupation[i] = 0;
	}*/

	return 0;
}

void start_mmu_C(void)
{
	register unsigned int control;
	__asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0)); //Disable cache
	__asm("mcr p15, 0, r0, c7, c7, 0"); //Invalidate cache (data and instructions) */
	__asm("mcr p15, 0, r0, c8, c7, 0"); //Invalidate TLB entries
	/* Enable ARMv6 MMU features (disable sub-page AP) */
	control = (1<<23) | (1 << 15) | (1 << 4) | 1;
	/* Invalidate the translation lookaside buffer (TLB) */
	__asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
	/* Write control register */
	__asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

}

void configure_mmu_C(void)
{
	register unsigned int pt_addr = FIRST_LVL_ADDR;
	total++;
	/* Translation table 0 */
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
	/* Translation table 1 */
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
	/* Use translation table 0 for everything */
	__asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
	/* Set Domain 0 ACL to "Manager", not enforcing memory permissions
	* Every mapped section/page is in domain 0
	*/
	__asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}

uint8_t* vMem_Alloc( unsigned int nbPages )
{
	return NULL;
}

void vMem_Free( uint8_t* ptr, unsigned int nbPages )
{

}