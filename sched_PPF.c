#include "sched_PPF.h"
#include "phyAlloc.h"
#include "hw.h"

struct pcb_s pcb_init;

void init_ctx(struct ctx_s* context, funct_t f, unsigned int size)
{
	context->stack_base = (uint32_t*) phyAlloc_alloc(size);
	context->sp = context->stack_base + (STACK_SIZE / 4) - 1;
	*(context->sp) = (uint32_t) 0x13;
	context->sp -= 1;
	*(context->sp) = (uint32_t) &start_current_process;
	context->sp -= 14;
	context->pc = f;
}

void init_pcb(struct pcb_s* pcb, funct_t f, unsigned int size, void* args,int priority)
{
	pcb->pcb_f = f;
	pcb->pcb_args = args;
	struct ctx_s* ctx= phyAlloc_alloc(sizeof(struct ctx_s));
	init_ctx(ctx,f,size);
	pcb-> pcb_ctx= ctx;
	pcb->state = NEW;
	pcb->priority = priority;
	pcb->initial_priority = priority;
	pcb->agingCpt=0;
	
}

void create_process(funct_t f, void* args, unsigned int stack_size,int priority)
{
	
	struct pcb_s* new_pcb = phyAlloc_alloc( sizeof(struct pcb_s));
	init_pcb(new_pcb,f,stack_size,args,priority);
	switch(priority)
	{
		case 0 :
			addToChainedList(new_pcb,listeCL[0]);
			break;
		case 1 :
			addToChainedList(new_pcb,listeCL[1]);
			break;
		case 2 :
			addToChainedList(new_pcb,listeCL[2]);
			break;
		case 3 :
			addToChainedList(new_pcb,listeCL[3]);
			break;
	}
	
	
}

void start_current_process()
{
	current_process->state = RUNNING;
	ENABLE_IRQ();
	set_tick_and_enable_timer();
	current_process->pcb_f();
	current_process->state = TERMINATED;
	while(1)
		;
}

void terminate_process()
{
	struct pcb_s* proc; 
	struct pcb_s* tmp;
	int i;
	for(i = 0 ; i < LISTECL_SIZE ; i++)
	{
		if(listeCL[i]->size > 0)
		{
			proc = listeCL[i]->first_pcb;
			while(proc->next_pcb != listeCL[i]->first_pcb)
			{
				while(proc->next_pcb->state!=TERMINATED && proc->next_pcb != listeCL[i]->first_pcb)
				{
					proc = proc->next_pcb;
				}
				if(proc->next_pcb->state==TERMINATED)
				{
					if(proc->next_pcb==listeCL[i]->first_pcb)
					{
						listeCL[i]->first_pcb=proc->next_pcb->next_pcb;
					}else if(proc->next_pcb==listeCL[i]->last_pcb)
					{
						listeCL[i]->last_pcb=proc;
					}
					tmp = proc->next_pcb->next_pcb;
					listeCL[i]->size--;
					phyAlloc_free(proc->next_pcb->pcb_ctx->stack_base,STACK_SIZE);
					phyAlloc_free(proc->next_pcb->pcb_ctx,sizeof(struct ctx_s));
					phyAlloc_free(proc->next_pcb, sizeof(struct pcb_s));
					proc->next_pcb = tmp;
				}
			}
			if(proc->next_pcb == proc && proc->state == TERMINATED)
			{
				listeCL[i]->size=0;
				phyAlloc_free(proc->pcb_ctx->stack_base,STACK_SIZE);
				phyAlloc_free(proc->pcb_ctx,sizeof(struct ctx_s));
				phyAlloc_free(proc, sizeof(struct pcb_s));
			}
		}
		
	}
}

void elect()
{
	
	if(current_process->priority == 0 && current_process->next_pcb !=current_process && current_process->next_pcb->state != TERMINATED)
	{
		if(current_process->state != TERMINATED)
		{
			current_process->state = WAITING;
		}
		current_process = current_process->next_pcb;
		return;
	}else{
		int i;
		for(i=0;i<=LISTECL_SIZE;i++)
		{
			if(listeCL[i]->size ==1 && listeCL[i]->first_pcb->state != JUST_WAITING && listeCL[i]->first_pcb->state != TERMINATED)
			{
				if(current_process->state != TERMINATED)
				{
					current_process->state = WAITING;
				}
				current_process = listeCL[i]->first_pcb;
				return;
			}else if(listeCL[i]->size>1)
			{
				struct pcb_s* tmp = listeCL[i]->first_pcb;
				do
				{
					if(tmp->state != JUST_WAITING && tmp->state != TERMINATED)
					{
						if(current_process->state != TERMINATED)
						{
							current_process->state = WAITING;
						}

						current_process = tmp;
						return;
					}else
					{
						tmp = tmp->next_pcb;
					}
					
				}
				while(tmp->next_pcb != listeCL[i]->first_pcb);
				
			}
		}	
	}
}

void aging()
{
	int i;
	for(i=0;i<LISTECL_SIZE;i++)
	{
		if(listeCL[i]->size != 0)
		{
			struct pcb_s* tmp = listeCL[i]->first_pcb;
			do
			{
				if(tmp->next_pcb->state==JUST_WAITING )
				{
					
					if(tmp->next_pcb->priority != tmp->next_pcb->initial_priority)
					{
						deleteFromChainedList(tmp->next_pcb,listeCL[i]);
						tmp->next_pcb->priority=tmp->next_pcb->initial_priority;
						addToChainedList(tmp->next_pcb,listeCL[tmp->next_pcb->priority]);
					}
					tmp->next_pcb->agingCpt = 0;
				}
				if(i != 0)
				{
					if(tmp->next_pcb->agingCpt < 3)
					{
						tmp->next_pcb->agingCpt++;
					}
					else
					{
						deleteFromChainedList(tmp->next_pcb,listeCL[i]);
						tmp->next_pcb->agingCpt = 0;
						tmp->next_pcb->priority--;
						addToChainedList(tmp->next_pcb,listeCL[i-1]);
						
					}
				}
				tmp = tmp->next_pcb;	
			}
			while(tmp->next_pcb->next_pcb!=listeCL[i]->first_pcb);		
		}
	}
}

void init_priority_list()
{
	priority0 = phyAlloc_alloc( sizeof(struct chained_list));
	listeCL[0] = priority0;
	listeCL[0]->priority = 0;
	listeCL[0]->size = 0;
	priority1 = phyAlloc_alloc( sizeof(struct chained_list));
	listeCL[1] = priority1;
	listeCL[1]->priority = 1;
	listeCL[1]->size = 0;
	priority2 = phyAlloc_alloc( sizeof(struct chained_list));
	listeCL[2] = priority2;
	listeCL[2]->priority = 2;
	listeCL[2]->size = 0;
	priority3 = phyAlloc_alloc( sizeof(struct chained_list));
	listeCL[3] = priority3;
	listeCL[3]->priority = 3;
	listeCL[3]->size = 0;
}

void start_sched_PPF()
{
	pcb_init.next_pcb = listeCL[0]->first_pcb;
	current_process = &pcb_init;
	ENABLE_IRQ();
	set_tick_and_enable_timer();
}

void ctx_switch_from_irq()
{
	//static int cpt = 0;
	DISABLE_IRQ();
	asm("sub lr, lr, #4");
	asm("srsdb sp!, #0x13");
	asm("cps #0x13");
	asm("push {r0-r12, lr}");

	asm("mov %0, sp" : "=r"(current_process->pcb_ctx->sp));

	current_process->state = JUST_WAITING;
	elect();
	aging();
	terminate_process();

	if(current_process->state == WAITING){
		current_process->state = RUNNING;
	}
		
	asm("mov sp, %0" : : "r"(current_process->pcb_ctx->sp));

	set_tick_and_enable_timer();

	asm("pop {r0-r12, lr}");
	
	ENABLE_IRQ();
	asm("rfeia sp!");

}


void addToChainedList(struct pcb_s* new_pcb, struct chained_list* CL)
{	
	if(CL->size==0)
	{
		CL->size++;
		new_pcb->next_pcb=new_pcb;
		CL->first_pcb=new_pcb;
		CL->last_pcb=new_pcb;
	}
	else 
	{
		CL->size++;
		CL->last_pcb->next_pcb=new_pcb;
		CL->last_pcb=new_pcb;
		CL->last_pcb->next_pcb=CL->first_pcb;
	}
}

void deleteFromChainedList(struct pcb_s* to_delete, struct chained_list* CL)
{
	if(CL->size>1)
	{
		struct pcb_s* tmp = CL->first_pcb;
		while(tmp->next_pcb!=to_delete)
		{
			tmp=tmp->next_pcb;
		}
		tmp->next_pcb=to_delete->next_pcb;
		if(tmp->next_pcb==CL->first_pcb)
		{
			CL->first_pcb=tmp->next_pcb->next_pcb;
		}else if(tmp->next_pcb==CL->last_pcb)
		{
			CL->last_pcb=tmp;
		}
	}
	CL->size--;	
}





