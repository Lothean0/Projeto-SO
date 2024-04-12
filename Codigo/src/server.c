#include "includes.h"
#include "mysystem.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        return -1;
    }

    char output_file_full[256];
    sprintf(output_file_full, "../%s/output_full.txt", argv[1]);
    char *fifopath = "../tmp/fifo";
    mkfifo(fifopath, 0666);
    int taskid = 1;
    int quit = 0;
    int fd_temp;
    int fd = open(fifopath, O_RDONLY);
    while (quit == 0)
    {
        Progam *args = malloc(sizeof(Progam));
        while ((read(fd, args, sizeof(Progam))) > 0)
        {

            if (strcmp(args->mode[0], "execute") == 0)
            {
                args->taskid = taskid++;
                char response_fifo[256];
                sprintf(response_fifo, "../tmp/response_fifo_%d", args->pid);
                int fd_response = open(response_fifo, O_WRONLY);
                write(fd_response, &args->taskid, sizeof(int));
                close(fd_response);
                if (fork() == 0)
                {
                    long tempo_exec = mysystem(args->command, args->taskid, argv[1]);
                    args->pid = getpid();
                    strcpy(args->mode[0], "fork");
                    args->tempo_exec = tempo_exec;
                    fd_temp = open(fifopath, O_WRONLY);
                    write(fd_temp, args, sizeof(Progam));
                    close(fd_temp);
                    exit(0);
                }
            }
            else if (strcmp(args->mode[0], "status") == 0)
            {
                printf("mode[0]: %s\n", args->mode[0]);
            }
            else if (strcmp(args->mode[0], "fork") == 0)
            {
                waitpid(args->pid, NULL, 0);
                char buffer[500];
                sprintf(buffer, "Taskid: %d\nProgama:%s\nTempo execução: %ld\n\n", args->taskid, args->command, args->tempo_exec);
                int fd_out = open(output_file_full, O_WRONLY | O_CREAT, 0666);
                lseek(fd_out, 0, SEEK_END);
                write(fd_out, buffer, strlen(buffer));
                close(fd_out);
            }
            else if (strcmp(args->mode[0], "quit") == 0)
            {
                quit = 1;
            }
        }
        free(args);
    }
    close(fd); // Moved outside of the while loop
    unlink(fifopath);
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