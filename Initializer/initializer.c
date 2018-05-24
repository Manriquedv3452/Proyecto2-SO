#include <stdio.h>
#include <sys/shm.h>
#include "../Utilities/utilities.c"

int main(int argc, char *argv[])
{
	printf("Inicializador: Inicio\n");

	// Se solicitan la cantidad de líneas por crear.
	int memory_spaces = atoi(argv[1]);

	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error en key\n");
		return -1; 
	}

	printf("Inicializador: Solicitando recursos para memoria compartida\n");

	int shmid = shmget(key, sizeof(struct sm_node), IPC_CREAT | 0777);
	if (shmid == -1)
    {
		fprintf (stderr, "Error al solicitar recursos.\n");
		return -1; 
	}

	printf ("Inicializador: ID Memoria Compartida: %d\n", shmid);

	struct sm_node* memory = shmat(shmid, NULL, 0);
	if (memory == NULL)
    {
		fprintf (stderr, "Error al reservar la memoria compartida.\n");
		return -1; 
	}

	//Uso el primer (extra) espacio para tener el total de espacios de la memoria
	memory[0].position = -1;
	memory[0].owner = memory_spaces;
	memory[0].num_segment = -1;
	memory[0].num_pag_seg = -1;

	//Inicializar memoria compartida en 0
	for (int i = 1; i < memory_spaces+1; i++) {
		memory[i].position = i-1;
		memory[i].owner = 0;
		memory[i].num_segment = 0;
		memory[i].num_pag_seg = 0;
	}

	printf("Inicializador: Fin\n");
	return(0);
}