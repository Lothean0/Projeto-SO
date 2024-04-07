#include "includes.h"

int main(int argc, char *argv[])
{
    char *fifopath = "fifos/fifo";
    char *args[argc-1];
    
    int fd = open(fifopath, O_WRONLY);
    write(fd,&argc,sizeof(int));
	for(int i=0; i < argc-1; i++){
		args[i] = strdup(argv[i+1]);
        char* pointer=args[i];
        write(fd,pointer,sizeof(char*));
        printf("arg[%d]: %s\n",i,pointer);
	}
    close(fd);
    return 0;
}