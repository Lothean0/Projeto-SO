#include "includes.h"
#include "mysystem.h"

int main()
{
    char *fifopath = "fifos/fifo";
    mkfifo(fifopath, 0666);
    int argc;
    ssize_t bytes_read;
    while(1)
    {
        int fd = open(fifopath, O_RDONLY);
        read(fd, &argc, sizeof(int));
        printf("argc: %d\n",argc);
        char *argv[argc-1];
        for (int j=0;(bytes_read = read(fd,argv[j], sizeof(char*))) > 0;j++);
        for (int i = 0; i < argc-1; i++)
            {
                printf("argv[%d]: %s\n",i,argv[i]);
                //write(0,argv[i],sizeof(argv[i]));
            }
        close(fd);
    }
    return 0;
}