#include "../types.h"
#include "vmem.h"


static uint32_t device_flags = 1079; 	//010000110111
static uint32_t kernel_flags = 1138; 	//010001110010
static uint32_t* first_level_entries;

void init_page_no_translation(uint32_t address, uint32_t sec_flag)
{
	uint32_t* first_level_descriptor = (uint32_t*)FIRST_DESCRIPTOR(address);
	if( (*first_level_descriptor & 3) == 0 )
	{
		// Ignored
		*first_level_descriptor = FIRST22(SEC_DESC_ADDR(address)) | 1;
	}
	uint32_t* second_level_descriptor = (uint32_t*)SECOND_DESCRIPTOR(address);
	// no translation
	*second_level_descriptor = FIRST20(address) | sec_flag;
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
	// A ce stade, on a aucune entrée dans la table
	/*uint32_t* first;
	uint32_t* second;
	uint32_t address = 0x0;
	int j;
	for( i = 0; i < FIRST_LVL_TT_COUN; ++i)
	{
		uint32_t* second_level_entries = SECON_LVL_ADDR + i*SECON_LVL_TT_SIZE;
		// Coarse page table adress
		// Les bits de poids fort de l'adresse des second level entries
		first_level_entries[i] = ((SECON_LVL_ADDR + i*SECON_LVL_TT_SIZE)>>10)<<10;
		first_level_entries[i] |= 1; // Flags
		for( j = 0; j < SECON_LVL_TT_COUN; ++j)
		{
			if( address <= USER_SPACE_BEGIN || (address >= DEVICE_ADDR_BEGIN && address <= DEVICE_ADDR_END))
			{
				second_level_entries[j] = address;
			}
			address += PAGE_SIZE;
		}
	}*/

	/*uint32_t address;
	for( address = 0; address < USER_SPACE_BEGIN; ++address)
	{
		init_page_no_translation(address, kernel_flags);
	}
	for( address = DEVICE_ADDR_BEGIN; address < DEVICE_ADDR_BEGIN; ++address)
	{
		init_page_no_translation(address, device_flags);
	}*/

	//init_page_no_translation(0x512,kernel_flags);

	uint32_t* pt = (uint32_t*)0x48000;
	*pt = (0x12345678&(~1024));
	
	return 0;
}
