#include "../Utilities/utilities.c"

void show_help(void);

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        show_help();
        return -1;
    }

    int memory_spaces = atoi(argv[1]);

    int request_sm = get_id_shared_memory(key, sizeof(struct sm_node)*(memory_spaces+1));
    if(request_sm < -1)
    {
        return -1;
    }
    
    struct sm_node* memory = shmat(request_sm, NULL, 0);
    if(memory == NULL)
    {
        perror("Error al obtener la memoria compartida.\n");
    }

    remove("activity-log.txt");

    memory[0].idLine = -1;
	memory[0].owner = memory_spaces;
	memory[0].num_segment = -1;
	memory[0].num_pag_seg = -1;

	int i;
	for (i = 1; i < memory_spaces+1; i++) {
		memory[i].idLine = i-1;
		memory[i].owner = 0;
		memory[i].num_segment = 0;
		memory[i].num_pag_seg = 0;
	}

    printf("\n[Memoria]:\n");

	for (i = 1; i < memory[0].owner + 1; i++) 
		printf("Posicion:%d\tProceso:%u\tNum-Seg(0 Pag):%d\tNum-Pag o Num-Seg:%d\n", memory[i].idLine, memory[i].owner, memory[i].num_segment, memory[i].num_pag_seg);
	
	printf("\n\n");

    write_activity_log(INITIALIZER, "Se ha preparado la memoria compartida.");

    return 0;
}

void show_help()
{
	printf("\nArgumentos Incorrectos. Las opciones son:\n\n");
	printf("\t/initializer <espacios de memoria a solicitar>\n\n");
}