#include "includes.h"

int main(int argc, char *argv[])
{
    int taskid;
    char response_fifo[256];
    pid_t pid = getpid();
    sprintf(response_fifo, "../tmp/response_fifo_%d", pid);
    if (mkfifo(response_fifo, 0666) == -1)
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    char *fifopath = "../tmp/fifo";
    if (argc < 2)
    {
        return -1;
    }

    if (strcmp(argv[1], "execute") == 0)
    {
        int fd = open(fifopath, O_WRONLY);
        if (fd == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        Progam *args = malloc(sizeof(Progam));
        args->pid = pid;
        args->argc = argc - 3;
        strcpy(args->mode[0], argv[1]);
        args->time = atoi(argv[2]);
        strcpy(args->mode[1], argv[3]);
        strcpy(args->command, argv[4]);
        for (int i = 5; i < argc; ++i)
        {
            strcat(args->command, " ");
            strcat(args->command, argv[i]);
        }
        printf("argc: %d\n", args->argc);
        printf("mode[0]: %s\n", args->mode[0]);
        printf("mode[1]: %s\n", args->mode[1]);
        printf("time: %d\n", args->time);
        printf("args: %s\n", args->command);

        write(fd, args, sizeof(Progam));
        close(fd);
        int fd_response = open(response_fifo, O_RDONLY);
        if (fd_response == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        while ((read(fd_response, &taskid, sizeof(int))) > 0);
        char taskid_str[256];
        sprintf(taskid_str, "Task_id: %d\n", taskid);
        write(1, taskid_str, strlen(taskid_str));
        close(fd_response);
        free(args);
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        int fd = open(fifopath, O_WRONLY);
        if (fd == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        Progam *args = malloc(sizeof(Progam));
        args->pid = pid;
        args->argc = 0;
        strcpy(args->mode[0], argv[1]);
        args->time = 0;
        strcpy(args->mode[1], "");
        strcpy(args->command, "");
        printf("mode[0]: %s\n", args->mode[0]);
        write(fd, args, sizeof(Progam));
        close(fd);
        free(args);
    }
    
    return 0;
}

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
