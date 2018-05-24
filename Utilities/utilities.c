#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>

#define PATHKEY "/dev/null"
#define KEY 9900
#define THREADS_LIMIT 300

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

#define SEMAPHORE "semaphore"

#define ACTIVITY_LOG "activity.log"
#define FILE_DEAD "dead.log"
#define FILE_END "finished.log"

#define BLOCK "block"
#define SEARCH "search"
#define SLEEP "sleep"

#define BUFF 1024

#define SEMAPHORE_FILE_LOG "semaphore_log"
#define SEMAPHORE_FILE_DEAD "semaphore_dead"
#define SEMAPHORE_FILE_END "semaphore_end"

#define TEST_BLOCK 0
#define SLEEP_BLOCK 1
#define TEST_SEARCH 0
#define SLEEP_SEARCH 1

struct sm_node
{
	int position;
	int owner;
	int num_segment; // Si es 0, es paginacion
	int num_pag_seg; // numero de pagina o segmento
};

struct semaphores
{
	sem_t* sem_shared_memory;
	sem_t* sem_activity_log;
	sem_t* sem_dead_log;
	sem_t* sem_finished_log;
};

void print_shared_memory(struct sm_node* memory)
{
    printf("\n[Memoria]:\n");

	for (int i = 1; i < memory[0].owner + 1; i++) 
		printf("Posicion:%d\tProceso:%u\tNum-Seg(0 Pag):%d\tNum-Pag o Num-Seg:%d\n", memory[i].position, memory[i].owner, memory[i].num_segment, memory[i].num_pag_seg);
	
	printf("\n\n");
}

void get_hour(char* output)
{
    char hour[2];
	char minutes[2];
	char seconds[2];

	time_t timer;
    time(&timer);
	struct tm* time = localtime(&timer);

	sprintf(hour, "%i", time->tm_hour);
	sprintf(minutes, "%i", time->tm_min);
	sprintf(seconds, "%i", time->tm_sec);
	
	sprintf(output,"%s:%s:%s", hour, minutes, seconds);
}

void write_to_file(char* file, char* data)
{
	char* temp;
	asprintf(&temp, "%s",data);

	FILE* log;
	log = fopen(file, "a");
	fprintf(log, "%s", temp);
	fclose(log);
}

void write_process_status(int PID, char * data)
{
    char* temp;
	asprintf(&temp, "%s", data);

	char* ffile;
	asprintf(&ffile, "%d", PID);

	FILE* pFile;
	pFile = fopen(ffile, "w");
	fprintf(pFile, "%s", temp);
	fclose(pFile);
}

void remove_file(int PID){
	char* ffile;
	asprintf(&ffile, "%d", PID);
	remove(ffile);
}