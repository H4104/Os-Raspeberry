#define PAGE_SIZE 		4096
#define SECON_LVL_TT_COUN	256
#define SECON_LVL_TT_SIZE	4*SECON_LVL_TT_COUN
#define FIRST_LVL_TT_COUN	4096
#define FIRST_LVL_TT_SIZE	4*FIRST_LVL_TT_COUN
#define TOTAL_TT_SIZE		(FIRST_LVL_TT_SIZE + SECON_LVL_TT_SIZE*FIRST_LVL_TT_COUN)

#define FIRST_LVL_ADDR          0x48000
#define SECON_LVL_ADDR          FIRST_LVL_ADDR + FIRST_LVL_SIZE

uint32_t device_flags;

#define UPDATE_FLAG(x) ( (x >= 0x20000000 && x <= 0x20FFFFFF)? device_flags = x&4096 : device_flags = 0 )

