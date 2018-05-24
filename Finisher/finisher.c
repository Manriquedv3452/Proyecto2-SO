#include "../Utilities/utilities.c"

int main(int argc, char *argv[])
{
	printf("Finalizador: Inicio\n");

	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error en key\n");
		return -1; 
	}

	int shmid = shmget(key, sizeof(struct sm_node), IPC_CREAT | 0777);
	if (shmid == -1)
    {
		fprintf (stderr, "Error al solicitar recursos.\n");
		return -1; 
	}

	struct sm_node* memory = shmat (shmid, NULL, 0);
	if (memory == NULL)
    {
		fprintf (stderr, "Error al reservar la memoria compartida.\n");
		return -1; 
	}

	printf("Finalizador: Detener proceso Productor\n");
	memory[0].position = 0;

	sem_unlink(SEMAPHORE);
	sem_unlink(SEMAPHORE_FILE_LOG);
	sem_unlink(SEMAPHORE_FILE_END);
	sem_unlink(SEMAPHORE_FILE_DEAD);

	printf ("Finalizador: Liberar memoria compartida\n");
	shmdt((char*)memory);
	shmctl(shmid, IPC_RMID, (struct shmid_ds*)NULL);

	printf("Finalizador: FIN\n");

	return(0);
}