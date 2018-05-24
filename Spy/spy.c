#include "../Utilities/utilities.c"

void memory_info();

int main()
{
    memory_info();
    return 0;
}

void memory_info()
{
	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error en key\n");
		return -1; 
	}

	printf("Espia: Solicitando recursos para memoria compartida\n");

	int shmid = shmget(key, sizeof(struct sm_node), IPC_CREAT | 0777);
	if (shmid == -1)
    {
		fprintf (stderr, "Error al solicitar recursos.\n");
		return -1; 
	}

	printf ("Espia: ID Memoria Compartida: %d\n", shmid);

	struct sm_node* memory = shmat(shmid, NULL, 0);
	if (memory == NULL)
    {
		fprintf (stderr, "Error al reservar la memoria compartida.\n");
		return -1; 
	}

	sem_t* sem;
	sem = sem_open(SEMAPHORE_MEMORY, O_RDWR, 0777, 1);

	// En el caso que el productor no esta corriendo, se crea
	if(sem == NULL)
    {
        sem = sem_open(SEMAPHORE_MEMORY, O_CREAT | O_EXCL, 0777, 1);
	}

	sem_wait(sem);

	printf("\nEspia: EL contenido de la memoria es:\n");

	for (int i = 1; i < memory[0].owner + 1; i++)
    {
        printf("\tPos:%d,\tProceso:%u,\tNum_Seg:%d,\tNum_Pag_Seg:%d\n",
        memory[i].pos, memory[i].owner, memory[i].num_segment, memory[i].num_pag_seg);
    }

	sem_post(sem);
}