#include "syscall.h"
#include "sched.h"

void sys_reboot(){
	__asm("mov r0, %0" : : "r"(1) : "r0");
	__asm("SWI 0" : : : "lr");
}

void sys_wait(unsigned int nbQuantums){
	__asm("mov r0, %0" : : "r"(1) : "r0");
	__asm("mov r1, %0" : : "r"(nbQuantums) : "r0");	
	__asm("SWI 0" : : : "lr");
}

void doSysCallReboot(){}

void doSysCallWait(){
	unsigned int nbQuantums;
	__asm("mov %0, r1" : "=r"(nbQuantums));
	
	current_process->tempsAttente=nbQuantums;
}

void SWIHandler(){
		int numSysCall=0;
		__asm("mov %0, r0" : "=r"(numSysCall));
		switch(numSysCall){
			case 1:
				doSysCallReboot();
				break;
			case 2:
				doSysCallWait();
				break;
		}
}
