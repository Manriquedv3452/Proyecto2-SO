#include <pthread.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "../Utilities/utilities.c"

void show_help();
int get_random(int min, int max);
void start_paging(struct semaphores* struct_sems);
void do_paging(int PID,int num_pages, struct semaphores * struct_sems);
int free_space(struct sm_node* mem);
void deallocate_memory(struct sm_node* mem, int PID);
int cantidadEspaciosLibresContiguos(struct sm_node* mem, int N);
void start_segmentation(struct semaphores* struct_sems);
void asignarSegmentacion(int PID,int num_segment, struct semaphores * struct_sems);

int process_id = 0;
pthread_t processes[THREADS_LIMIT];

int main(int argc, char *argv[])
{
	srand(time(NULL));
	
    int segmentation = 0;
    int paging = 0;

    if(argc == 2)
    {
        if (strcmp(argv[1], "-pag") == 0)
        {
            paging = 1;
        }
        
        else if (strcmp(argv[1], "-seg") == 0)
        {
            segmentation = 1;
        }
    }
    else
    {
        show_help();
		return -1;
    }

	remove("activity.log");
	remove("finished.log");
	remove("dead.log");

	sem_t* sem_sm;
	sem_sm = sem_open(SEMAPHORE_MEMORY, O_CREAT | O_EXCL, 0777, 1);

	sem_t* sem_activity_log;
	sem_activity_log = sem_open(SEMAPHORE_FILE_LOG, O_CREAT | O_EXCL, 0777, 1);

	sem_t* sem_finished_log;
	sem_finished_log = sem_open(SEMAPHORE_FILE_DEAD, O_CREAT | O_EXCL, 0777, 1);

	sem_t* sem_dead_log;
	sem_dead_log = sem_open(SEMAPHORE_FILE_END, O_CREAT | O_EXCL, 0777, 1);

	struct semaphores struct_sems;
	struct_sems.sem_shared_memory = sem_sm;
	struct_sems.sem_activity_log = sem_activity_log;
	struct_sems.sem_dead_log = sem_finished_log;
	struct_sems.sem_finished_log = sem_dead_log;

	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error en key\n");
		return -1; 
	}

	printf("Productor: Solicitando recursos para memoria compartida\n");
    int shmid = shmget(key, sizeof(struct sm_node), IPC_CREAT | 0777);
	if (shmid == -1)
    {
		fprintf (stderr, "Error al solicitar recursos.\n");
		return -1; 
	}

	struct sm_node* memory = shmat(shmid, NULL, 0);
	if (memory == NULL)
    {
		fprintf (stderr, "Error al reservar la memoria compartida.\n");
		return -1; 
	}

	/*write_process_status(PID, BLOCK);
    if(TEST_BLOCK)
    {
		sleep(SLEEP_BLOCK);
	}
	*/
	sem_wait(struct_sems.sem_shared_memory);

	int threads_index = 0;

	// Crear thread para el proceso
    while(memory[0].position == -1)
    {
		sem_post(struct_sems.sem_shared_memory);
        pthread_t newthread;

		if(segmentation == 1)
        {
			if (pthread_create(&processes[threads_index], NULL, start_segmentation, &struct_sems) != 0)
				perror("pthread_create");

		}
        else if(paging == 1)
        {
			if (pthread_create(&processes[threads_index], NULL, start_paging, &struct_sems) != 0)
				perror("pthread_create");
		}

		threads_index++;

		// pausa entre ejecucion de procesos
		int sleep_time = get_random(MIN_TIME_CREATE_PROCESS, MAX_TIME_CREATE_PROCESS);
		printf("Productor: Sleep del proceso -> %d\n", sleep_time);
		sleep(sleep_time);

		sem_wait(struct_sems.sem_shared_memory);
	}

	sem_post(struct_sems.sem_shared_memory);

	sem_destroy(sem_sm);
	sem_destroy(sem_activity_log);
	sem_destroy(sem_finished_log);
	sem_destroy(sem_dead_log);

	for (int i = 0; i < threads_index; i++)
	{
		pthread_join(processes[i], NULL);
	}

	return(0);
}

void show_help()
{
	printf("\nArgumentos Incorrectos. Las opciones son:\n\n");
	printf("\t./producer -pag para paginacion\n");
    printf("\t./producer -seg para segmentacion\n\n");
}

int get_random(int min, int max){
	int num;
	num = rand();
	num = num % (max - min + 1);
	return (num + min);
}

void start_paging(struct semaphores* struct_sems)
{
	process_id++;
	int num_pages = get_random(MIN_AMOUNT_PAGES, MAX_AMOUNT_PAGES);
	printf("Productor: Nuevo proceso %d con %d paginas.\n", process_id, num_pages);

	do_paging(process_id, num_pages, struct_sems);

	pthread_cancel(pthread_self());
}

void do_paging(int PID, int num_pages, struct semaphores* struct_sems){

    int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error en key\n");
		return; 
	}

	printf("Productor: Solicitando recursos para memoria compartida\n");
    int shmid = shmget(key, sizeof(struct sm_node), IPC_CREAT | 0777);
	if (shmid == -1)
    {
		fprintf (stderr, "Error al solicitar recursos.\n");
		return; 
	}

	struct sm_node* memory = shmat(shmid, NULL, 0);
	if (memory == NULL)
    {
		fprintf (stderr, "Error al reservar la memoria compartida.\n");
		return; 
	}

	/*write_process_status(PID, BLOCK);
    if(TEST_BLOCK)
    {
		sleep(SLEEP_BLOCK);
	}
	*/
	sem_wait(struct_sems->sem_shared_memory);

	/*write_process_status(PID, SEARCH);
    if(TEST_SEARCH)
    {
		sleep(SLEEP_SEARCH);
	}
	*/
	int free_space_mem = free_space(memory);
	printf("\nProductor: Espacios libres en memoria compartida -> %d.\n", free_space_mem);
	
    if(num_pages <= free_space_mem)
    {
        int asigned_spaces = 0;
		for(int i = 1; i < memory[0].owner + 1; i++)
        {
			// Espacio disponible
			if( memory[i].owner == 0 && 
                memory[i].num_segment == 0 && 
                memory[i].num_pag_seg == 0)
            {

                // Asignacion
                asigned_spaces++;
                memory[i].owner = PID;
                memory[i].num_segment = 0;
                memory[i].num_pag_seg = asigned_spaces;

                char hour[8];
                get_hour(hour);

                char* data;
                asprintf(&data, "Proceso:%d\tAsignacion\t%s\tPag\tPos:%d\tAsig:%d\n",
                        PID, hour, memory[i].position, asigned_spaces);

                sem_wait(struct_sems->sem_activity_log);
                write_to_file(ACTIVITY_LOG, data);
                sem_post(struct_sems->sem_activity_log);

                if(asigned_spaces == num_pages )
                {
                    printf("Productor: Asignacion de paginas en memoria terminada.\n");
                    break;
                }
            }
		}

	}
    
    else
    {
		printf("Productor: No hay espacio disponible para el proceso -> %d\n", PID);

		char hour[8];
		get_hour(hour);

		char* data;
		asprintf(&data, "Proceso:%d\tNo hay espacio\t%s\tPag\t-\n", PID, hour);

		sem_wait(struct_sems->sem_activity_log);
		write_to_file(ACTIVITY_LOG, data);
		sem_post(struct_sems->sem_activity_log);

		char* data_dead;
		asprintf(&data_dead, "%d\n", PID);
		sem_wait(struct_sems->sem_dead_log);
		write_to_file(FILE_DEAD,data_dead);
		sem_post(struct_sems->sem_dead_log);

		remove_file(PID);

		sem_post(struct_sems->sem_shared_memory);
		return;
	}

	sem_post(struct_sems->sem_shared_memory);

	write_process_status(PID, SLEEP);
	
    // Simulacion de ejecucion del proceso
	sleep(get_random(MIN_TIME_PROCESS, MAX_TIME_PROCESS));

	write_process_status(PID, BLOCK);
	
    sem_wait(struct_sems->sem_shared_memory);
	
	deallocate_memory(memory, PID);

	char hour[8];
	get_hour(hour);

	char* data;
	asprintf(&data, "Proceso:%d\tDesasignacion\t%s\tPag\t-\n", PID, hour);

	sem_wait(struct_sems->sem_activity_log);
	write_to_file(ACTIVITY_LOG,data);
	sem_post(struct_sems->sem_activity_log);

	sem_post(struct_sems->sem_shared_memory);

	char* data_finished;
	asprintf(&data_finished, "%d\n", PID);
	sem_wait(struct_sems->sem_finished_log);
	write_to_file(FILE_END, data_finished);
	sem_post(struct_sems->sem_finished_log);

	remove_file(PID);
}

int free_space(struct sm_node* mem){
	int i, count = 0;
	for(i = 1; i < mem[0].owner +1; i++){
		if(mem[i].owner == 0 && 
			mem[i].num_segment == 0 && 
			mem[i].num_pag_seg == 0)
			count++;
	}
	return count;
}

void deallocate_memory(struct sm_node* memory, int PID)
{
	for(int i = 1; i < memory[0].owner + 1; i++)
    {
		if(memory[i].owner == PID)
        {
			memory[i].owner = 0;
			memory[i].num_segment = 0;
			memory[i].num_pag_seg = 0;
		}
	}
}

void start_segmentation(struct semaphores* struct_sems)
{
    return;
}