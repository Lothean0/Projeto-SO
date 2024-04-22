#include "includes.h"

int main(int argc, char *argv[])
{
    int taskid;
    pid_t pid = getpid();
    char *fifopath = "../tmp/fifo";
    if (argc < 2)
    {
        return -1;
    }

    if (strcmp(argv[1], "execute") == 0)
    {
        char response_fifo[256];
        sprintf(response_fifo, "../tmp/response_fifo_%d", pid);
        if (mkfifo(response_fifo, 0666) == -1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
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
        args->tempo_exec = 0;
        for (int i = 5; i < argc; ++i)
        {
            strcat(args->command, " ");
            strcat(args->command, argv[i]);
        }

        write(fd, args, sizeof(Progam));
        close(fd);
        int fd_response = open(response_fifo, O_RDONLY);
        if (fd_response == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        while ((read(fd_response, &taskid, sizeof(int))) > 0)
            ;
        char taskid_str[256];
        sprintf(taskid_str, "Task_id: %d\n", taskid);
        write(1, taskid_str, strlen(taskid_str));
        close(fd_response);
        free(args);
        unlink(response_fifo);
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        char response_fifo_exec[256];
        char response_fifo_sched[256];
        char response_fifo_done[256];
        sprintf(response_fifo_exec, "../tmp/response_fifo_%d_exec", pid);
        sprintf(response_fifo_sched, "../tmp/response_fifo_%d_sched", pid);
        sprintf(response_fifo_done, "../tmp/response_fifo_%d_done", pid);
        if (mkfifo(response_fifo_exec, 0666) == -1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
        if (mkfifo(response_fifo_sched, 0666) == -1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
        if (mkfifo(response_fifo_done, 0666) == -1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
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
        args->tempo_exec = 0;
        write(fd, args, sizeof(Progam));
        close(fd);
        int fd_response_exec = open(response_fifo_exec, O_RDONLY);
        if (fd_response_exec == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        int fd_response_sched = open(response_fifo_sched, O_RDONLY);
        if (fd_response_sched == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        int fd_response_done = open(response_fifo_done, O_RDONLY);
        if (fd_response_done == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        Progam *execprogs = malloc(sizeof(Progam));
        int execprogs_size = 1;
        int execprogs_count = 0;
        while (read(fd_response_exec, args, sizeof(Progam)) > 0)
        {
            if (execprogs_count == execprogs_size)
            {
                execprogs_size *= 2;
                execprogs = realloc(execprogs, execprogs_size * sizeof(Progam));
            }
            execprogs[execprogs_count] = *args;
            execprogs_count++;
        }
        Progam *schedprogs = malloc(sizeof(Progam));
        int schedprogs_size = 1;
        int schedprogs_count = 0;
        while (read(fd_response_sched, args, sizeof(Progam)) > 0)
        {
            if (schedprogs_count == schedprogs_size)
            {
                schedprogs_size *= 2;
                schedprogs = realloc(schedprogs, schedprogs_size * sizeof(Progam));
            }
            schedprogs[schedprogs_count] = *args;
            schedprogs_count++;
        }
        free(args);
        
        write(1, "Executing\n", strlen("Executing:\n"));
        for (int i = 0; i < execprogs_count; i++)
        {
            char execprogs_str[500];
            sprintf(execprogs_str, "%d %s\n", execprogs[i].taskid, execprogs[i].command);
            write(1, execprogs_str, strlen(execprogs_str));
        }
        write(1, "\nScheduled\n", strlen("Scheduled:\n"));
        for (int i = 0; i < schedprogs_count; i++)
        {
            char schedprogs_str[500];
            sprintf(schedprogs_str, "%d %s\n", schedprogs[i].taskid, schedprogs[i].command);
            write(1, schedprogs_str, strlen(schedprogs_str));
        }
        char donefilepath[256];
        while (read(fd_response_done, donefilepath, 256) > 0) {}
        write(1, "\nCompleted\n", strlen("\nCompleted\n"));
        int fd_done = open(donefilepath, O_RDONLY); // Open the file specified by donefilepath
        if (fd_done != -1){
        ssize_t bytesRead;
        while ((bytesRead = read(fd_done, donefilepath, 256)) > 0)
        {
            write(1, donefilepath, bytesRead);
        }
        }
        close(fd_done);
        close(fd_response_exec);
        close(fd_response_sched);
        close(fd_response_done);
        free(execprogs);
        free(schedprogs);
        unlink(response_fifo_exec);
        unlink(response_fifo_sched);
        unlink(response_fifo_done);
    }
    else if (strcmp(argv[1], "quit") == 0)
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
        args->tempo_exec = 0;
        write(fd, args, sizeof(Progam));
        close(fd);
        free(args);
    }
    return 0;
}