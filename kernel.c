#include "sched.h"
#include "hw.h"
#include "vmem/vmem.h"



void funcA() {
	int	cptA = 0;
	while( 1 ) {
		//int* pt = (int *)vMem_Alloc(1);
		//pt[0] = ++cptA;
		//vMem_Free(pt,1);
		//pt = (int*)35642;
		cptA++;
	}
}
void funcB(){
	int	cptB = 1;
	while( 1 ) {
		//int* pt = (int *)vMem_Alloc(1);
		//pt[0] = ++cptB;
		//vMem_Free(pt,1);
		//pt = (int*)3546;
		cptB++;
	}
}
//------------------------------------------------------------------------
int kmain (void){
	init_hw();
	//init_kern_translation_table();

	//configure_mmu_C();
	//start_mmu_C();

	//uint32_t* pt = (uint32_t*)vMem_Alloc(1);
	//*pt = sizeof(uint32_t);
	//uint32_t* pt = 0x500000;

	create_process(funcA, NULL, STACK_SIZE);
	create_process(funcB, NULL, STACK_SIZE);
	start_sched();
	while(1){}
/* Pas atteignable vues nos 2 fonctions */
	return 0;
}
