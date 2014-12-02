#include "sched.h"
#include "phyAlloc.h"
#include "hw.h"


void init_ctx(struct ctx_s* context, funct_t f, unsigned int size){
	context->init_sp = phyAlloc_alloc(size);
	context->sp = context->init_sp + STACK_SIZE-4;
	context->pc = f;
}

/*void __attribute__ ((naked)) switch_to(struct ctx_s* ctx ){
	__asm("push {r0-r12}");
	__asm("mov %0, sp" : "=r"(current_ctx->sp));
	__asm("mov %0, lr" : "=r"(current_ctx->pc));
	
	current_ctx = ctx;
	
	__asm("mov sp, %0" : : "r"(current_ctx->sp));
	__asm("mov lr, %0" : : "r"(current_ctx->pc));
	__asm("pop {r0-r12}");
	__asm("bx lr");
}
*/
void init_pcb(struct pcb_s* pcb, funct_t f, unsigned int size, void* args){
	pcb->pcb_f = f;
	pcb->pcb_args = args;
	struct ctx_s* ctx= phyAlloc_alloc(sizeof(struct ctx_s));
	init_ctx(ctx,f,size);
	pcb-> pcb_ctx= ctx;
	pcb->state = NEW;
}

void create_process(funct_t f, void* args, unsigned int stack_size){
	
	struct pcb_s* new_pcb = phyAlloc_alloc( sizeof(struct pcb_s));
	init_pcb(new_pcb,f,stack_size,args);
	
	if(CL.first_pcb->state != NEW && CL.first_pcb->state !=WAITING && CL.first_pcb->state !=TERMINATED && CL.first_pcb->state !=RUNNING){
		CL.first_pcb=new_pcb;
		CL.first_pcb=new_pcb;
		CL.last_pcb=new_pcb;
	}
	else {
		CL.last_pcb->next_pcb=new_pcb;
		CL.last_pcb=new_pcb;
		CL.last_pcb->next_pcb=CL.first_pcb;
	}
}

void start_current_process(){
	ENABLE_IRQ();
	set_tick_and_enable_timer();
	current_process->state = RUNNING;
	current_process->pcb_f();
	DISABLE_IRQ();
	current_process->state = TERMINATED;
}

void terminate_process(){
	struct pcb_s* proc = CL.first_pcb;
	struct pcb_s* tmp;
	
	while(proc->next_pcb != CL.first_pcb){
		while(proc->next_pcb->state!=TERMINATED && proc->next_pcb != CL.first_pcb){
			proc = proc->next_pcb;
		}
		if(proc->next_pcb->state==TERMINATED){
			tmp = proc->next_pcb->next_pcb;
			phyAlloc_free(proc->next_pcb->pcb_ctx->init_sp,STACK_SIZE);
			phyAlloc_free(proc->next_pcb->pcb_ctx,sizeof(struct ctx_s));
			phyAlloc_free(proc->next_pcb, sizeof(struct pcb_s));
			proc->next_pcb = tmp;
		}
	}
	
	/**
	if(proc != proc->next_pcb){
		while(proc->next_pcb->state != TERMINATED){
			proc = proc->next_pcb;
		}
		
		proc->next_pcb = proc->next_pcb->next_pcb;
	}
	phyAlloc_free(pcb->pcb_ctx->init_sp,STACK_SIZE);
	phyAlloc_free(pcb->pcb_ctx,sizeof(struct ctx_s));
	phyAlloc_free(pcb, sizeof(struct pcb_s));*/
}

void elect(){
	current_process = current_process->next_pcb;
}

void start_sched(){
	ENABLE_IRQ();
	set_tick_and_enable_timer();
	current_process=CL.first_pcb;
}

void ctx_switch_from_irq(){
	DISABLE_IRQ();
	asm("sub lr, lr, #4");
	asm("srsdb sp!, #0x13");
	asm("cps #0x13");
	
	if(current_process != NULL && current_process->state!=NEW){
		asm("push {r0-r12}");
		asm("mov %0, sp" : "=r"(current_process->pcb_ctx->sp));
		asm("mov %0, lr" : "=r"(current_process->pcb_ctx->pc));
		current_process->state=WAITING;
	}
	elect();
	if(current_process->state == TERMINATED){
		struct pcb_s* tmp = current_process;
		while(tmp->next_pcb->state == TERMINATED){
			tmp = tmp->next_pcb;
		}
		current_process = tmp->next_pcb;
		terminate_process();
	}
	while(current_process->state == NEW)
	{
		start_current_process();
		elect();
	}
	if(current_process->state == WAITING){
		current_process->state = RUNNING;
	}
	asm("mov sp, %0" : : "r"(current_process->pcb_ctx->sp));
	asm("mov lr, %0" : : "r"(current_process->pcb_ctx->pc));
	asm("pop {r0-r12}");
	
	ENABLE_IRQ();
	set_tick_and_enable_timer();
	asm("rfeia sp!");
}


void __attribute__((naked)) ctx_switch(){
	if(current_process->state == NEW)
	{
		start_current_process();
	}else if(current_process->state == TERMINATED){
		struct pcb_s* tmp = current_process;
		while(tmp->next_pcb->state == TERMINATED){
			tmp = tmp->next_pcb;
		}
		current_process = tmp->next_pcb;
		terminate_process();
	}else{
		__asm("push {r0-r12}");
		__asm("mov %0, sp" : "=r"(current_process->pcb_ctx->sp));
		__asm("mov %0, lr" : "=r"(current_process->pcb_ctx->pc));
		current_process->state=WAITING;
		
		elect();
	}
	if(current_process->state == NEW)
	{
		start_current_process();
	}
	if(current_process->state == WAITING){
		current_process->state = RUNNING;
		__asm("mov sp, %0" : : "r"(current_process->pcb_ctx->sp));
		__asm("mov lr, %0" : : "r"(current_process->pcb_ctx->pc));
		__asm("pop {r0-r12}");
		__asm("bx lr");
	}	
}
