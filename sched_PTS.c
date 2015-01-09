#include "sched_PTS.h"
#include "phyAlloc.h"
#include "hw.h"

struct pcb_s pcb_init;

void init_ctx(struct ctx_s* context, funct_t f, unsigned int size){
	context->stack_base = (uint32_t*) phyAlloc_alloc(size);
	context->sp = context->stack_base + (STACK_SIZE / 4) - 1;
	*(context->sp) = (uint32_t) 0x13;
	context->sp -= 1;
	*(context->sp) = (uint32_t) &start_current_process;
	context->sp -= 14;
	context->pc = f;
}

void init_pcb(struct pcb_s* pcb, funct_t f, unsigned int size, void* args,int priority){
	pcb->pcb_f = f;
	pcb->pcb_args = args;
	struct ctx_s* ctx= phyAlloc_alloc(sizeof(struct ctx_s));
	pcb->tempsAttente = 0;
	init_ctx(ctx,f,size);
	pcb-> pcb_ctx= ctx;
	pcb->state = NEW;
	pcb->priority = priority;
}

void create_process(funct_t f, void* args, unsigned int stack_size,int priority){
	struct pcb_s* new_pcb = phyAlloc_alloc( sizeof(struct pcb_s));
	init_pcb(new_pcb,f,stack_size,args,priority);
	if(CL.first_pcb->state != NEW && CL.first_pcb->state !=WAITING && CL.first_pcb->state !=TERMINATED && CL.first_pcb->state !=RUNNING){
		CL.first_pcb=new_pcb;
		CL.first_pcb=new_pcb;
		CL.last_pcb=new_pcb;
	}else {
		CL.last_pcb->next_pcb=new_pcb;
		CL.last_pcb=new_pcb;
		CL.last_pcb->next_pcb=CL.first_pcb;
	}
}

void start_current_process(){ 
	ENABLE_IRQ(); 
	set_tick_and_enable_timer_TS(current_process->priority);
	current_process->state = RUNNING; 
	current_process->pcb_f(); 
	current_process->state = TERMINATED; 
	while(1) 
	;
}

void terminate_process(){
	struct pcb_s* proc = CL.first_pcb;
	struct pcb_s* tmp;
	while(proc->next_pcb != CL.first_pcb){
		while(proc->next_pcb->state!=TERMINATED && proc->next_pcb != CL.first_pcb){
			proc = proc->next_pcb;
			if(proc==proc->next_pcb)
			{
				CL.first_pcb=proc;
			}
		}
		if(proc->next_pcb->state==TERMINATED){
			tmp = proc->next_pcb->next_pcb;
			phyAlloc_free(proc->next_pcb->pcb_ctx->stack_base,STACK_SIZE);
			phyAlloc_free(proc->next_pcb->pcb_ctx,sizeof(struct ctx_s));
			phyAlloc_free(proc->next_pcb, sizeof(struct pcb_s));
			proc->next_pcb = tmp;
		}
	}
}

void update_waiting()
	{
	struct pcb_s* tmp = current_process;
	do
	{
		if((tmp->tempsAttente)>0)
		{
			(tmp->tempsAttente)--;
		}
		tmp=(tmp->next_pcb);
	}while(tmp != current_process);
}

void elect(){
	struct pcb_s* tmp = current_process;
	do{
		if(current_process==tmp && current_process!= &pcb_init)
		{
			update_waiting();
		}
		current_process = current_process->next_pcb;
	}while(current_process->tempsAttente>0);
}

void start_sched(){
	
	pcb_init.next_pcb = CL.first_pcb;
	current_process=&pcb_init;
	ENABLE_IRQ();
	set_tick_and_enable_timer_TS(3);
	
}

void ctx_switch_from_irq(){
	
	DISABLE_IRQ();
	asm("sub lr, lr, #4");
	asm("srsdb sp!, #0x13");
	asm("cps #0x13");	
	asm("push {r0-r12 , lr}");
	asm("mov %0, sp" : "=r"(current_process->pcb_ctx->sp));
	if(current_process->state != TERMINATED){
		current_process->state=WAITING;
	}
	struct pcb_s* tmp;
	elect();
	if(current_process->state == TERMINATED){
		tmp = current_process;
		while(tmp->next_pcb->state == TERMINATED){
			tmp = tmp->next_pcb;
		}
		current_process = tmp->next_pcb;
		terminate_process();
	}
	
	if(current_process->state == WAITING){
		current_process->state = RUNNING;
	}
	asm("mov sp, %0" : : "r"(current_process->pcb_ctx->sp));
	set_tick_and_enable_timer_TS(current_process->priority);
	asm("pop {r0-r12,lr}");
	ENABLE_IRQ();
	
	asm("rfeia sp!");
}


