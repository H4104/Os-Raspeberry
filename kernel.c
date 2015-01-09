#include "sched_PPF.h"
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
	int	cptb = 1;
	while( 1 ) {
		cptb ++;
		if(cptb>15000){
			cptb = 0;
		}
	}
}
void funcC(){
	int	cptC = 1;
	while( 1 ) {
		cptC ++;
		if(cptC>3){
			return;
		}
	}
}

//------------------------------------------------------------------------
int kmain (void){
	init_hw();
	init_priority_list();
	create_process(funcA, NULL, STACK_SIZE,0);
	create_process(funcB, NULL, STACK_SIZE,0);
	//create_process(funcC, NULL, STACK_SIZE,0);
	start_sched_PPF();
	while(1){}
/* Pas atteignable vues nos 2 fonctions */
	return 0;
}
