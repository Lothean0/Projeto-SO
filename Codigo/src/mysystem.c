#include "includes.h"

// recebe um comando por argumento
// retorna -1 se o fork falhar
// caso contrario retorna o valor do comando executado
int mysystem (const char* command, int taskid) {

	int res = -1;
	pid_t pid;
	char output_file[256];
	sprintf(output_file, "../tmp/output_task_id_%d.txt", taskid);
	int fd=open(output_file, O_WRONLY | O_CREAT , 0666);
	// Estamos a assumir numero maximo de argumentos
	// isto teria de ser melhorado com realloc por exemplo
	char *exec_args[20];
	char *string, *cmd, *tofree;
	int i=0;
	tofree = cmd = strdup(command);
	while((string = strsep(&cmd," "))!=NULL){
	   exec_args[i]=string;
	   i++;
	}
	exec_args[i]=NULL;

	
	struct tempo comeco,fim;
	long segundos, microseg;

	gettimeofday(&comeco,NULL);
	
	if ((pid=fork())==0)
    	{
        int res = execvp(exec_args[0],exec_args);
        if (res==-1)perror("No such file");
        _exit(res);
    	}
	else
	{
		wait(&res);
		res = WEXITSTATUS(res);
	}
	gettimeofday(&fim,NULL);
	segundos = fim.segundos - comeco.segundos;
	microseg = fim.microseg - comeco.microseg;
	long tempoTotal = (segundos * 1000) + (microseg/1000); // convertendo pra milisegundos
	printf("tempo de execucao: %ld milisegundos\n",tempoTotal); // print aqui so pra debug, dps tirar
	
	
	free(tofree);

	return res;
}


//redirecionar aqui o ficheiro
//mudar a mysystem para receber a taskid
//sprintf(response_fifo, "../tmp/task_id_%d",&taskid);
