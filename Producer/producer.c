#include "../Utilities/utilities.c"
#include <pthread.h>

int process_id = 0;

void show_help(void);
void paging_process(void);
void segmentation_process(void);
void* thread_paging_process();
int get_random(int min, int max);
int available_space();
void allocate_memory(int process_id, int num_pages);
void deallocate_memory(int process_id);
void write_activity_log(char* message);

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        show_help();
        return -1;
    }

    srand(time(NULL));

    int paging = 0;
    int segmentation = 0;

    if (strcmp(argv[1],"-seg") == 0)
    {
        segmentation = 1;
    }

    if (strcmp(argv[1],"-pag") == 0){
        paging = 1;
    }

    int request_sm = get_id_shared_memory(key, sizeof(struct sm_node));
    
    printf("ID MEMORIA: %d\n\n", request_sm);

    if(request_sm < -1)
    {
        return -1;
    }

    int i;

    memory = shmat(request_sm, NULL, 0);

    print_shared_memory(memory);

    sem_memory = sem_open("Procesos", O_CREAT, 0666, 1);
    sem_init(sem_memory, 0, 1);

    sem_log = sem_open("Bitacora", O_CREAT, 0666, 1);
    sem_init(sem_log, 0, 1);

    if(paging == 1)
    {
        paging_process();
    }
    else if (segmentation == 1)
    {
        segmentation_process();
    }

    return 0;
}

void show_help()
{
	printf("\nArgumentos Incorrectos. Las opciones son:\n\n");
	printf("\t/producer -pag para paginacion\n");
    printf("\t/producer -seg para segmentacion\n\n");
}

void paging_process()
{
    // mientras el finisher no haya terminado la memoria
    while(memory[0].owner != 0)
    {
        pthread_t* new_process;
        new_process = (pthread_t*)malloc(sizeof(*new_process));

        if(pthread_create(&new_process, NULL, (void*)thread_paging_process, NULL) != 0)
        {
            perror("Error al crear thread de proceso.");
        }

        //sleep para la pausar creacion de procesos
        sleep(get_random(MIN_TIME_CREATE_PROCESS, MAX_TIME_CREATE_PROCESS));
    }
}

void* thread_paging_process()
{
    int num_pages = get_random(MIN_AMOUNT_PAGES, MAX_AMOUNT_PAGES);

    int request_sm = get_id_shared_memory(key, sizeof(struct sm_node));
    
    if(request_sm < -1)
    {
        perror("Error al obtener memoria compartida.");
        return;
    }

    memory = shmat(request_sm, NULL, 0);
    if(memory == NULL)
    {
        perror("Error al obtener memoria compartida.");
        return;
    }

    process_id++;
    printf("\nEl proceso %d ha sido creado con %d paginas.\n", process_id, num_pages);
    printf("El proceso espera el candado de la memoria\n");

    //Obtener candado para usar memoria compartida
    sem_wait(sem_memory);

    allocate_memory(process_id, num_pages);

    //liberar candado de memoria compartida
    sem_post(sem_memory);

    //simular ejecucion de proceso
    sleep(get_random(MIN_TIME_PROCESS, MAX_TIME_PROCESS));
    sem_wait(sem_memory);
    deallocate_memory(process_id);

    sem_post(sem_memory);

    pthread_cancel(pthread_self());
}

void segmentation_process()
{
    return;
}

int get_random(int min, int max)
{
	int num;
	num = rand();
	num = num % (max - min + 1);
	return (num + min);
}

int available_space_memory()
{
    int i, count = 0;
	for(i = 1; i < memory[0].owner + 1; i++)
    {
		if( memory[i].owner == 0 && 
            memory[i].num_segment == 0 && 
			memory[i].num_pag_seg == 0)
        {
            count++;
        }
	}

	return count;
}

void allocate_memory(int process_id, int num_pages)
{
    printf("\nBuscando ubicacion para las paginas\n");
    int space_memory = available_space_memory();
    printf("Espacio Disponible: %d\n", space_memory);
    
    if(num_pages <= space_memory)
    {
        int allocated_spaces = 0;

        for(int i = 1; i < memory[0].owner + 1; i++)
        {
            if( memory[i].owner == 0 && 
                memory[i].num_segment == 0 && 
			    memory[i].num_pag_seg == 0)
            {
                allocated_spaces++;
                memory[i].owner = process_id;
                memory[i].num_segment = PAGING;
                memory[i].num_pag_seg = allocated_spaces;

                //escribir en bitacora
                char* message;
				asprintf(&message, "PID:%d \tAsignacion \tEspacio: ", process_id);
                sem_wait(sem_log);
                write_activity_log(message);
                sem_post(sem_log);

                if(allocated_spaces == num_pages)
                {
                    printf("\nPaginas asignadas al proceso %d\n", process_id);
                    break;
                }
            }
        }
    }
    else
    {
        printf("\nNo hay espacio suficiente para el proceso %d\n", process_id);
        char* message;
        asprintf(&message, "No hay espacio suficiente para el proceso %d. El proceso muere.", process_id);
        write_activity_log(message);
        sem_post(sem_log);
    }
}

void deallocate_memory(int process_id)
{
    for(int i = 1; i < memory[0].owner +1; i++)
    {
		if(memory[i].owner == process_id)
        {
            memory[i].owner = 0;
			memory[i].num_segment = 0;
			memory[i].num_pag_seg = 0;
		}
	}

    //escribir en bitacora
    char* message;
    asprintf(&message, "PID:%d \tDesasignacion \tEspacio: ", process_id);
    write_activity_log(message);
}

void write_activity_log(char* message) {

    FILE* log;
    char filename[] = "activity-log.txt";
    log = fopen(filename, "a");
    
    if (log == NULL){
        perror("No se puede abrir la bitacora.\n"); 
    }
    
    time_t timer;
    time(&timer);
    struct tm* time = localtime(&timer);
    sem_wait(sem_log);

    fprintf(log, "\nProductor:\n");
    fprintf(log, "\tMensaje: %s\n", message);
    fprintf(log, "\tHora: %i:%i:%i\n", time->tm_hour, time->tm_min, time->tm_sec);

     //fprintf(log, "Productor\n PID: %i; Tipo accion: %s; Hora: %i:%i:%i; Linea: %i\n", PID, "asignacion", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, line);
    
    sem_post(sem_log);
    fclose(log);

    printf("\n**** Se escribe en bitacora: %s ****\n", message);
}