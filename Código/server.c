#include "includes.h"
#include "mysystem.h"

int main()
{
    char *fifopath = "fifos/fifo";
    mkfifo(fifopath, 0666);
    ssize_t bytes_read;
    while(1)
    {
        Progam *args=malloc(sizeof(Progam));
        int fd = open(fifopath, O_RDONLY);
        
        while((bytes_read = read(fd,args, sizeof(Progam))) > 0);
        printf("argc: %d\n",args->argc);
        printf("mode[0]: %s\n",args->mode[0]);
        printf("mode[1]: %s\n",args->mode[1]);
        printf("time: %d\n",args->time);
        printf("args: %s\n",args->command);
        mysystem(args->command);
        close(fd);
    }
    return 0;
}

/*
int main()
{
    char *fifopath = "fifos/fifo";
    mkfifo(fifopath, 0666);

    while (1)
    {
        int fd = open(fifopath, O_RDONLY);
        if (fd == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }

        int argc;
        read(fd, &argc, sizeof(int));
        printf("argc: %d\n", argc);
        char *args[argc];
        int exec_args_len = 0;
        char *exec_args = NULL;
        for (int i = 0; i < argc; i++)
        {
            size_t arg_len;
            read(fd, &arg_len, sizeof(size_t));
            args[i] = (char *)malloc(arg_len + 1);
            if (args[i] == NULL)
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            read(fd, args[i], arg_len);
            args[i][arg_len] = '\0';
            printf("argv[%d]: %s\n", i, args[i]);
            // Resize exec_args buffer and copy the argument
            exec_args = realloc(exec_args, exec_args_len + arg_len + 1 + 1); // +1 for space between arguments, +1 for null terminator
            if (exec_args == NULL)
            {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
            if (i>0)
            {
                exec_args[exec_args_len-1]=' ';
            }
            strcat(exec_args, args[i]);
            exec_args_len += arg_len;

            free(args[i]);
        }
        exec_args[exec_args_len - 1] = '\0'; // Null terminate the whole string
        printf("exec_args: %s\n", exec_args);
        mysystem(exec_args);
        free(exec_args);
        // Close FIFO
        close(fd);
    }
    return 0;
}
*/