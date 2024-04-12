#include "includes.h"

// recebe um comando por argumento
// retorna -1 se o fork falhar
// caso contrario retorna o valor do comando executado
long mysystem(const char *command, int taskid, char *output_folder)
{

	int res = -1;
	pid_t pid;
	char output_file[256];
	sprintf(output_file, "../%s/output_task_id_%d.txt", output_folder, taskid);
	printf("output_folder: %s\n", output_folder);
	int fd = open(output_file, O_WRONLY | O_CREAT, 0666);
	dup2(fd, STDOUT_FILENO);
	close(fd);
	dup2(fd, STDERR_FILENO);
	// Estamos a assumir numero maximo de argumentos
	// isto teria de ser melhorado com realloc por exemplo
	char *exec_args[20];
	char *string, *cmd, *tofree;
	int i = 0;
	tofree = cmd = strdup(command);
	while ((string = strsep(&cmd, " ")) != NULL)
	{
		exec_args[i] = string;
		i++;
	}
	exec_args[i] = NULL;

	struct tempo comeco, fim;
	long segundos, microseg;

	gettimeofday(&comeco, NULL);

	if ((pid = fork()) == 0)
	{
		int res = execvp(exec_args[0], exec_args);
		if (res == -1)
			perror("No such file");
		_exit(res);
	}
	dup2(STDOUT_FILENO, STDOUT_FILENO);
	wait(&res);
	res = WEXITSTATUS(res);
	gettimeofday(&fim, NULL);
	segundos = fim.segundos - comeco.segundos;
	microseg = fim.microseg - comeco.microseg;
	long tempoTotal = (segundos * 1000) + (microseg / 1000);	 // convertendo pra milisegundos
	//printf("tempo de execucao: %ld milisegundos\n", tempoTotal); // print aqui so pra debug, dps tirar
	close(fd);
	free(tofree);

	return tempoTotal;
}

// redirecionar aqui o ficheiro
// mudar a mysystem para receber a taskid
// sprintf(response_fifo, "../tmp/task_id_%d",&taskid);
