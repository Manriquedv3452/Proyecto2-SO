#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>

#define KEY 9999

#define INITIALIZER 0
#define PRODUCER 1
#define SPY 2
#define FINISHER 3
#define PAGING 0

#define MIN_AMOUNT_PAGES 1
#define MAX_AMOUNT_PAGES 10

#define MIN_AMOUNT_SEGMENTS 1
#define MAX_AMOUNT_SEGMENTS 5

#define MIN_AMOUNT_SPACES_SEGMENTS 1
#define MAX_AMOUNT_SPACES_SEGMENTS 3

#define MIN_TIME_PROCESS 1 //20 segs
#define MAX_TIME_PROCESS 3 //60 segs

#define MIN_TIME_CREATE_PROCESS 1 //30 segs
#define MAX_TIME_CREATE_PROCESS 3 //60 segs

struct sm_node {
	int position;
	unsigned long owner;
	int num_segment; // Si es 0, es paginacion
	int num_pag_seg; // numero de pagina o segmento
};

key_t key = 9990;
struct sm_node* memory;
static sem_t* sem_memory;
static sem_t* sem_log;

int get_id_shared_memory(key_t key, int size)
{
    int shmid;
    shmid = shmget(key, size, IPC_CREAT | 0666);

    if(shmid < 0)
    {
        perror("Error al inicializar memoria compartida.");
        return -1;
    }

    return shmid;
}

void print_shared_memory(struct sm_node* memory)
{
    printf("\n[Memoria]:\n");

	for (int i = 1; i < memory[0].owner + 1; i++) 
		printf("Posicion:%d\tProceso:%u\tNum-Seg(0 Pag):%d\tNum-Pag o Num-Seg:%d\n", memory[i].position, memory[i].owner, memory[i].num_segment, memory[i].num_pag_seg);
	
	printf("\n\n");
}