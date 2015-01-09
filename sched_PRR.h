
#define STACK_SIZE 2048
#define NULL 0

typedef enum State State;

typedef void (*funct_t)(void);

enum State
{
	NEW,WAITING,RUNNING,TERMINATED
};

struct ctx_s{
	void* sp;
	void* pc;
	void* init_sp;
};

struct pcb_s{
	struct pcb_s* next_pcb;
	void* pcb_args;
	funct_t pcb_f;
	unsigned int tempsAttente;	
	struct ctx_s* pcb_ctx;
	State state;
	void* address;
	int priority;
};

struct chained_list{
	struct pcb_s* first_pcb;
	struct pcb_s* last_pcb;
};

struct pcb_s* current_process;
struct chained_list CL;

void init_pcb(struct pcb_s* context, funct_t f, unsigned int size, void* args, int priority );

//void __attribute__ ((naked)) switch_to(struct ctx_s* context);

void __attribute__((naked)) ctx_switch();

void ctx_switch_from_irq();

void init_ctx(struct ctx_s* context, funct_t f, unsigned int size);

void create_process(funct_t f, void* args, unsigned int stack_size, int priority);

void start_current_process();

void terminate_process();

void elect();

void start_sched();
