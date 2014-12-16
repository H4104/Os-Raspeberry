#include "syscall.h"
#include "sched_PRR.h"
#include "hw.h"

void sys_reboot(){
	DISABLE_IRQ();	
	__asm("mov r0, %0" : : "r"(1) : "r0");
	__asm("SWI 0" : : : "lr");
	ENABLE_IRQ();	
}

void sys_wait(unsigned int nbQuantums){
	DISABLE_IRQ();
	__asm("mov r0, %0" : : "r"(2) : "r0");
	__asm("mov r1, %0" : : "r"(nbQuantums) : "r1");
	__asm("SWI 0" : : : "lr");
	ENABLE_IRQ();		
}

void doSysCallReboot(){
	const int PM_RSTC = 0x2010001c;
	const int PM_WDOG = 0x20100024;
	const int PM_PASSWORD = 0x5a000000;
	const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
	PUT32(PM_WDOG, PM_PASSWORD | 1);
	PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
	while(1);
}

void doSysCallWait(){
	unsigned int nbQuantums;
	__asm("mov %0, r1" : "=r"(nbQuantums));	
	current_process->tempsAttente=nbQuantums+1;
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
