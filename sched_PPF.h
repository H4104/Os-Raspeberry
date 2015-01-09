#define STACK_SIZE 2048
#define NULL 0
#define LISTECL_SIZE 4
#define AGING_LIMIT 5


typedef enum State State;

typedef void (*funct_t)(void);



enum State
{
	NEW,WAITING,RUNNING,TERMINATED,JUST_WAITING
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
	struct ctx_s* pcb_ctx;
	State state;
	void* address;
	int priority;
	int initial_priority;
	int agingCpt;
};

struct chained_list{
	int priority;
	struct pcb_s* first_pcb;
	struct pcb_s* last_pcb;
	int size;
};

struct pcb_s* current_process;

struct chained_list* listeCL [LISTECL_SIZE];
struct chained_list* priority0;
struct chained_list* priority1;
struct chained_list* priority2;
struct chained_list* priority3;


void start_sched_PPF();

void init_pcb(struct pcb_s* context, funct_t f, unsigned int size, void* args , int priority);

void init_ctx(struct ctx_s* context, funct_t f, unsigned int size);

void create_process(funct_t f, void* args, unsigned int stack_size,int priority);

void addToChainedList(struct pcb_s* new_pcb, struct chained_list* CL);

void deleteFromChainedList(struct pcb_s* to_delete, struct chained_list* CL);

void start_current_process();

void terminate_process();

void init_priority_list();

void elect();

void aging();
