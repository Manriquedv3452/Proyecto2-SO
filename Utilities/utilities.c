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

#define MIN_MEM_SPACES_SEG 1
#define MAX_MEM_SPACES_SEG 3

#define MIN_TIME_PROCESS 1 //20 segs
#define MAX_TIME_PROCESS 3 //60 segs

#define MIN_TIME_CREATE_PROCESS 1 //30 segs
#define MAX_TIME_CREATE_PROCESS 3 //60 segs

#define SEMAPHORE_MEMORY "semaphore"

#define ACTIVITY_LOG "activity.log"
#define FILE_DEAD "dead.log"
#define FILE_END "finished.log"

#define BLOCK "block"
#define SEARCH "search"
#define SLEEP "sleep"

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
	int num_segment; // Si es -2, es paginacion
	int num_pag_seg; // numero de pagina u offset
};

struct semaphores
{
	sem_t* sem_shared_memory;
	sem_t* sem_activity_log;
	sem_t* sem_dead_log;
	sem_t* sem_finished_log;
};

void get_hour(char* output)
{
    char hour[2];
	char minutes[2];
	char seconds[2];

	time_t timer;
    time(&timer);
	struct tm* time = localtime(&timer);

	sprintf(hour, "%d", time->tm_hour);
	sprintf(minutes, "%d", time->tm_hour);
	sprintf(seconds, "%d", time->tm_hour);
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