#include "includes.h"

int main(int argc, char *argv[])
{
    char *fifopath = "fifos/fifo";
    Progam *args=malloc(sizeof(Progam));
    args->argv=malloc(sizeof(*args->argv)*argc);
	int N = 0;
	for(int i=1; i < argc; i++){
		args->argv[N] = strdup(argv[i]);
		N++;
	}
    args->argc=argc;
    printf("argc: %d\n",args->argc);
    for (int i = 0; i < argc-1; i++)
    {
        printf("arg[%d]: %s\n",i,args->argv[i]);
    }
    
    int fd = open(fifopath, O_WRONLY);
    write(fd,args,sizeof(Progam)+sizeof(*args->argv)*argc);
    close(fd);
    return 0;
}