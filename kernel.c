#include "hw.h"
#include "uart.h"
#include "vmem/vmem.h"
#include "syscall.h"
#include "sched_PTS.h"

void funcA() {
	int	cptA = 0;
	sys_wait(3);
	while(1) {
		cptA ++;
  		uart_send_str("funcA\n"); 
		if(cptA>1500000){
			cptA = 0;
		}
	}
}


void funcB(){
	int	cptB = 1;
	while(1) {
		cptB += 2;
  		uart_send_str("funcB\n"); 
		if(cptB>150000000){
			cptB=0;
		}
	}
}

void functC(){
	int cptC = 0;
  	uart_send_str("funcC\n"); 
	if(cptC == 15){
		return;
	}else{
		cptC++;
	}
}

int kmain (void){
	init_hw();
  	uart_init();
  	
	/*-- INPUT --*/
	//char buffer[32];
  	//uart_receive_str(buffer, 32);  //read

	/*-- PROCESS --*/
	create_process(funcA, NULL, STACK_SIZE,0);
	create_process(funcB, NULL, STACK_SIZE,0);
	create_process(functC,NULL,STACK_SIZE,1);
	
	/*-- virtual memory --*/
	//init_kern_translation_table();

	//configure_mmu_C();
	//start_mmu_C();

	//uint32_t* pt = (uint32_t*)vMem_Alloc(1);
	//*pt = sizeof(uint32_t);
	//uint32_t* pt = 0x500000;
	
	/*-- start --*/
	start_sched();
	while(1){}
	/* Pas atteignable vues nos 2 fonctions */
	return 0;
  
}
