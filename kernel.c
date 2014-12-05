#include "sched_PRR.h"
#include "hw.h"



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
			cptB += 2 ;
			if(cptB>150000000){
				cptB=0;
			}
	}
}
//------------------------------------------------------------------------
int kmain (void){
	init_hw();
	create_process(funcA, NULL, STACK_SIZE);
	create_process(funcB, NULL, STACK_SIZE);
	start_sched();
	while(1){}
/* Pas atteignable vues nos 2 fonctions */
	return 0;
}
