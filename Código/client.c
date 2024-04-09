#include "includes.h"

///*
int main(int argc, char *argv[])
{
    char *fifopath = "fifos/fifo";
    if (argc<2)
    {
        return -1;
    }
    
    // Open FIFO for writing
    int fd = open(fifopath, O_WRONLY);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    if (strcmp(argv[1],"execute")==0)
    {
        Progam *args = malloc(sizeof(Progam));
        args->argc = argc-3;
        strcpy(args->mode[0], argv[1]);
        args->time = atoi(argv[2]);
        strcpy(args->mode[1], argv[3]);
        strcpy(args->command, argv[4]);
        for (int i = 5; i < argc; ++i)
        {
            strcat(args->command, " ");
            strcat(args->command, argv[i]);
        }
        printf("argc: %d\n",args->argc);
        printf("mode[0]: %s\n",args->mode[0]);
        printf("mode[1]: %s\n",args->mode[1]);
        printf("time: %d\n",args->time);
        printf("args: %s\n",args->command);
        
        write(fd, args, sizeof(Progam));
    }
    //else{}

    // Close FIFO
    close(fd);

    return 0;
}
//*/
/*
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
*/
