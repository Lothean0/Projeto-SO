#include "includes.h"
#include "mysystem.h"

int main()
{
    char *fifopath = "fifos/fifo";
    mkfifo(fifopath, 0666);
    ssize_t bytes_read;
    Progam *args=malloc(sizeof(Progam));
    while (1)
    {
        int fd = open(fifopath, O_RDONLY);
        //problema em passar array
        while ((bytes_read = read(fd, args, sizeof(Progam))) > 0)
        {
            for (int i = 0; i < args->argc-1; i++)
            {
                write(0,args->argv[i],sizeof(char));
            }
        }
        close(fd);
    }
    return 0;
}