#include "includes.h"
#include "mysystem.h"
#include <math.h>

int main(int argc, char *argv[])
{
    char output_file_full[256];
    sprintf(output_file_full, "../%s/output_full.txt", argv[1]);
    char *fifopath = "../tmp/fifo";
    mkfifo(fifopath, 0666);
    ssize_t bytes_read;
    Task *task = malloc(sizeof(Task));
    task->taskid = 1;
    while (1)
    {
        Progam *args = malloc(sizeof(Progam));
        int fd = open(fifopath, O_RDONLY);

        while ((bytes_read = read(fd, args, sizeof(Progam))) > 0)
            ;

        if (strcmp(args->mode[0], "execute") == 0)
        {
            int fd_pipe[2], fd_pipe2[2];
            pipe(fd_pipe);
            pipe(fd_pipe2);
            if (fork() == 0)
            {
                strcpy(task->command, args->command);
                char response_fifo[256];
                sprintf(response_fifo, "../tmp/response_fifo_%d", args->pid);
                int fd_response = open(response_fifo, O_WRONLY);
                write(fd_response, &task->taskid, sizeof(int));
                close(fd_response);
                close(fd_pipe[0]);
                write(fd_pipe[1], task, sizeof(Task));
                exit(0);
            }
            for (int i = 0; i < atoi(argv[2]); i++)
            {
                if (fork() == 0)
                {
                    close(fd_pipe[1]);
                    read(fd_pipe[0], task, sizeof(Task));
                    long tempo_exec = mysystem(task->command, task->taskid, argv[1]);
                    int fd_out = open(output_file_full, O_WRONLY | O_CREAT, 0666);
                    char *taskid_char = malloc(sizeof(char) * floor(log10(task->taskid)) + 8);
                    char *tempo_exec_char = malloc(sizeof(char) * floor(log10(tempo_exec)) + 19);
                    sprintf(taskid_char, "Taskid: %d", task->taskid);
                    sprintf(tempo_exec_char, "Tempo execução: %ld", tempo_exec);
                    lseek(fd_out, 0, SEEK_END);
                    write(fd_out, taskid_char, floor(log10(task->taskid) + 9));
                    write(fd_out, "\n", sizeof(char));
                    write(fd_out, tempo_exec_char, floor(log10(tempo_exec) + 19));
                    write(fd_out, "\n", sizeof(char));
                    write(fd_out, "\n", sizeof(char));
                    close(fd_pipe[0]);
                    close(fd_out);
                }
            }
            close(fd_pipe[1]);
                    task->taskid++;
            // printf("pid: %d\n", args->pid);
            // printf("argc: %d\n", args->argc);
            // printf("mode[0]: %s\n", args->mode[0]);
            // printf("mode[1]: %s\n", args->mode[1]);
            // printf("time: %d\n", args->time);
            // printf("args: %s\n", args->command);
        }
        else if (strcmp(args->mode[0], "status") == 0)
        {
            printf("mode[0]: %s\n", args->mode[0]);
        }
        close(fd);
        free(args);
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