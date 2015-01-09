#include "sched_PTS.h"
#include "hw.h"
#include "syscall.h"

void funcA() {
	int	cptA = 0;
	//sys_wait(3);
	while(1) {
		cptA ++;
		if(cptA>1500000){
			cptA = 0;
		}
	}
}
void funcB(){
	int	cptB = 1;
	while(1) {
			cptB += 2 ;
			if(cptB>150000000){
				cptB=0;
			}
	}
}

void functC(){
	int cptC = 0;
	if(cptC == 15000000){
		return;
	}else{
		cptC++;
	}
}
//------------------------------------------------------------------------
int kmain (void){
	init_hw();
	create_process(funcA, NULL, STACK_SIZE,3);
	create_process(funcB, NULL, STACK_SIZE,0);
	create_process(functC,NULL,STACK_SIZE,2);
	start_sched();
	while(1){}
/* Pas atteignable vues nos 2 fonctions */
	return 0;
}
