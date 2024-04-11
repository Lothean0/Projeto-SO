#include "includes.h"

// recebe um comando por argumento
// retorna -1 se o fork falhar
// caso contrario retorna o valor do comando executado

//esta é a certa
int mysystem (const char* command) {

	int res = -1;
	pid_t pid;

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
	free(tofree);

	return res;
}
