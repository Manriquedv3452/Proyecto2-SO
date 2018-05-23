#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define INITIALIZER 0
#define PRODUCER 1
#define SPY 2
#define FINISHER 3

key_t key = 9990;
int* memory;

struct sm_node {
	int idLine;
	unsigned long owner;
	int num_segment; // Si es 0, es paginacion
	int num_pag_seg; // numero de pagina o segmento
};

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

void write_activity_log(int writer, char* message) {

    FILE* log;
    char filename[] = "activity-log.txt";
    log = fopen(filename, "a");
    
    if (log == NULL){
        perror("No se puede abrir la bitacora.\n"); 
    }
    
    time_t timer;
    time(&timer);
    struct tm* time = localtime(&timer);
    
    switch(writer)
    {
        case INITIALIZER:
            fprintf(log, "\nInicializador:\n");
            fprintf(log, "\tMensaje: %s\n", message);
            fprintf(log, "Hora: %i:%i:%i\n", time->tm_hour, time->tm_min, time->tm_sec);
            break;
        default:
            break;
    }
    
    //fprintf(log, "Productor\n PID: %i; Tipo accion: %s; Hora: %i:%i:%i; Linea: %i\n", PID, "asignacion", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, line);
    
    fclose(log);
}