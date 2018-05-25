#include "../Utilities/utilities.c"

void memory_info();
void processes_info();

int main(int argc, char *argv[])
{
	memory_info();
    return 0;
}

void memory_info()
{
	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error en key\n");
		return; 
	}

	printf("Espia: Solicitando recursos para memoria compartida\n");

	int shmid = shmget(key, sizeof(struct sm_node), IPC_CREAT | 0777);
	if (shmid == -1)
    {
		fprintf (stderr, "Error al solicitar recursos.\n");
		return; 
	}

	printf ("Espia: ID Memoria Compartida: %d\n", shmid);

	struct sm_node* memory = shmat(shmid, NULL, 0);
	if (memory == NULL)
    {
		fprintf (stderr, "Error al reservar la memoria compartida.\n");
		return; 
	}

	sem_t* sem;
	sem = sem_open(SEMAPHORE_MEMORY, O_RDWR, 0777, 1);

	// En el caso que el productor no esta corriendo, se crea
	if(sem == NULL)
    {
        printf("\nNo hay memoria compartida en este momento.\n");
		return;
	}

	sem_wait(sem);

	printf("\nEspia: EL contenido de la memoria es:\n\n");

	if(memory[1].num_segment == -2)
	{
		for (int i = 1; i < memory[0].owner + 1; i++)
		{
        	printf("\tPosicion:%d \tProceso:%d\t Num_Pagina:%d\n",
        	memory[i].position, memory[i].owner, memory[i].num_pag_seg);
    	}
	}

	else
	{
		for (int i = 1; i < memory[0].owner + 1; i++)
		{
        	printf("\tPosicion:%d \tProceso:%d\t Num_Segmento:%d \tOffset:%d\n",
        	memory[i].position, memory[i].owner, memory[i].num_segment, memory[i].num_pag_seg);
    	}
	}

	

	printf("\n");
	if(memory[0].num_segment != -1)
	{
		printf("\nEspia: Proceso buscando memoria -> %d\n\n", memory[0].num_segment);
	}
	sem_post(sem);
}