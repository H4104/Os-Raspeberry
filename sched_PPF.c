#include "sched_PPF.h"
#include "phyAlloc.h"
#include "hw.h"

void init_ctx(struct ctx_s* context, funct_t f, unsigned int size){
	context->init_sp = phyAlloc_alloc(size);
	context->sp = context->init_sp + STACK_SIZE-4;
	context->pc = f;
}

void init_pcb(struct pcb_s* pcb, funct_t f, unsigned int size, void* args,int priority){
	pcb->pcb_f = f;
	pcb->pcb_args = args;
	struct ctx_s* ctx= phyAlloc_alloc(sizeof(struct ctx_s));
	init_ctx(ctx,f,size);
	pcb-> pcb_ctx= ctx;
	pcb->state = NEW;
	pcb->priority = priority;
	
}

void create_process(funct_t f, void* args, unsigned int stack_size,int priority){
	
	struct pcb_s* new_pcb = phyAlloc_alloc( sizeof(struct pcb_s));
	init_pcb(new_pcb,f,stack_size,args,priority);
	switch(priority){
		case 1 :
			addToChainedList(new_pcb,listeCL[0]);
			break;
		case 2 :
			addToChainedList(new_pcb,listeCL[1]);
			break;
		case 3 :
			addToChainedList(new_pcb,listeCL[2]);
			break;
		case 4 :
			addToChainedList(new_pcb,listeCL[3]);
			break;
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
	struct pcb_s* proc = listeCL[].first_pcb;
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
}

void elect(){
	current_process = current_process->next_pcb;
}

void start_sched(){
	
	listeCL[0] = &priority0;
	listeCL[0]->priority = 0;
	listeCL[1] = &priority1;
	listeCL[1]->priority = 1;
	listeCL[2] = &priority2;
	listeCL[2]->priority = 2;
	listeCL[3] = &priority3;
	listeCL[3]->priority = 3;
		
	
	ENABLE_IRQ();
	set_tick_and_enable_timer();
	
}

void ctx_switch_from_irq(){

}


void addToChainedList(struct pcb_s* new_pcb, struct chained_list* CL){
	
	if(CL->first_pcb->state != NEW && CL->first_pcb->state !=WAITING && CL->first_pcb->state !=TERMINATED && CL->first_pcb->state !=RUNNING && CL->first_pcb->state != JUST_WAITING){
			CL->first_pcb=new_pcb;
			CL->last_pcb=new_pcb;
		}
		else {
			CL->last_pcb->next_pcb=new_pcb;
			CL->last_pcb=new_pcb;
			CL->last_pcb->next_pcb=CL->first_pcb;
		}
}
