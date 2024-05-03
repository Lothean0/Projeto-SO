#include "includes.h"

long mysystem(const char *command, int taskid, char *output_folder, struct tempo comeco)
{
	struct tempo fim;
	long segundos, microseg;

	int res = -1;
	pid_t pid;
	char output_file[256];
	sprintf(output_file, "../%s/output_task_id_%d", output_folder, taskid);
	int fd = open(output_file, O_WRONLY | O_CREAT, 0666);
	

	if (fd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	dup2(fd, STDOUT_FILENO);
	

	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		exit(EXIT_FAILURE);
	}
	close(fd);
	
	fd = open(output_file, O_WRONLY | O_CREAT, 0666);
	dup2(fd, STDERR_FILENO);
	
	if (dup2(fd, STDERR_FILENO) == -1)
	{
		perror("dup2");
		exit(EXIT_FAILURE);
	}
	
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

	if ((pid = fork()) == 0)
	{
		int res = execvp(exec_args[0], exec_args);
		if (res == -1)
			perror("No such file");
		_exit(res);
	}
	dup2(STDOUT_FILENO, STDOUT_FILENO);
	
	if (dup2(STDOUT_FILENO, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		exit(EXIT_FAILURE);
	}
	dup2(STDERR_FILENO, STDERR_FILENO);
	
	if (dup2(STDERR_FILENO, STDERR_FILENO) == -1)
	{
		perror("dup2");
		exit(EXIT_FAILURE);
	}
	wait(&res);
	res = WEXITSTATUS(res);
	close(fd);
	free(tofree);
	gettimeofday(&fim, NULL);
	segundos = fim.segundos - comeco.segundos;
	microseg = fim.microseg - comeco.microseg;
	long tempoTotal = (segundos * 1000) + (microseg / 1000); // converte pra milisegundos
	return tempoTotal;
}

