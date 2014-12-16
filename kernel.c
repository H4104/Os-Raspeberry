#include "sched.h"
#include "hw.h"
#include "vmem/vmem.h"



void funcA() {
	int	cptA = 0;
	while( 1 ) {
		cptA ++;
		if(cptA>1500000){
			cptA = 0;
		}
	}
}
void funcB(){
	int	cptB = 1;
	while( 1 ) {
			cptB += 2 ;
	}
}
//------------------------------------------------------------------------
int kmain (void){
	init_hw();
	init_kern_translation_table();

	configure_mmu_C();
	start_mmu_C();

	uint32_t* pt = (uint32_t*)vMem_Alloc(1);
	*pt = sizeof(uint32_t);

	pt[1023] = 42;
	pt[1024] = 43; // crashes :)

	uint32_t* pt2 = (uint32_t*)vMem_Alloc(1);

	/*create_process(funcA, NULL, STACK_SIZE);
	create_process(funcB, NULL, STACK_SIZE);
	start_sched();*/
	while(1){}
/* Pas atteignable vues nos 2 fonctions */
	return 0;
}
