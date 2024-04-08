#include "includes.h"

/*
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
*/

int main(int argc, char *argv[]) {
    char *fifopath = "fifos/fifo";
    
    // Open FIFO for writing
    int fd = open(fifopath, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    
    int argc_to_write = argc - 1; // Exclude program name
    write(fd, &argc_to_write, sizeof(int));
    
    // Write each argument starting from argv[1]
    for (int i = 1; i < argc; i++) {
        size_t arg_len = strlen(argv[i]) + 1; // Include null terminator
        write(fd, &arg_len, sizeof(size_t));
        write(fd, argv[i], arg_len);
    }
    
    // Close FIFO
    close(fd);
    
    return 0;
}


