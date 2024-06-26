#include "includes.h"
#include "mysystem.h"
#include "exec_pipes.h"

void enqueue(FCFS_Task **queue, Progam task)
{
    FCFS_Task *new_task = (FCFS_Task *)malloc(sizeof(FCFS_Task));
    new_task->task = task;
    new_task->next = NULL;

    if (*queue == NULL)
    {                      
        *queue = new_task; 
    }
    else
    {
        FCFS_Task *current = *queue; 
        while (current->next != NULL)
        {                            
            current = current->next; 
        }
        current->next = new_task; 
    }
}
void enqueue_sjf(FCFS_Task **queue, Progam task)
{
    FCFS_Task *new_task = (FCFS_Task *)malloc(sizeof(FCFS_Task));
    new_task->task = task;
    new_task->next = NULL;

    if (*queue == NULL)
    {                      
        *queue = new_task; 
    }
    else
    {
        FCFS_Task *current = *queue;
        FCFS_Task *prev = NULL;
        while (current != NULL && current->task.time < task.time)
        { 
            prev = current;
            current = current->next;
        }
        if (prev == NULL)
        { 
            new_task->next = *queue;
            *queue = new_task;
        }
        else
        { 
            prev->next = new_task;
            new_task->next = current;
        }
    }
}

void enqueue_ljf(FCFS_Task **queue, Progam task)
{
    FCFS_Task *new_task = (FCFS_Task *)malloc(sizeof(FCFS_Task));
    new_task->task = task;
    new_task->next = NULL;

    if (*queue == NULL)
    {                      
        *queue = new_task; 
    }
    else
    {
        FCFS_Task *current = *queue;
        FCFS_Task *prev = NULL;
        while (current != NULL && current->task.time > task.time)
        { 
            prev = current;
            current = current->next;
        }
        if (prev == NULL)
        { 
            new_task->next = *queue;
            *queue = new_task;
        }
        else
        { 
            prev->next = new_task;
            new_task->next = current;
        }
    }
}

Progam dequeue(FCFS_Task **queue)
{
    FCFS_Task *temp = *queue; 
    Progam task = temp->task; 
    *queue = (*queue)->next;  
    free(temp);               
    return task;              
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        const char *message = "Usage: ";
        write(STDERR_FILENO, message, strlen(message));
        write(STDERR_FILENO, argv[0], strlen(argv[0]));
        const char *message2 = " output_folder parallel-tasks sched-policy\n";
        write(STDERR_FILENO, message2, strlen(message2));
        return -1;
    }
    struct stat st = {0};
    char output_folder[256];
    sprintf(output_folder, "../%s", argv[1]);
    if (stat(output_folder, &st) == -1)
    {
        mkdir(output_folder, 0700);
    }
    char output_file_full[256];
    sprintf(output_file_full, "../%s/output_full", argv[1]);
    mkfifo(SERVER, 0666);
    int taskid = 1;
    int fd = open(SERVER, O_RDONLY);
    
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    int fd_write = open(SERVER, O_WRONLY);
    
    if (fd_write == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    char *sched_policy = argv[3];
    int max_parallel_tasks = atoi(argv[2]);
    int running_tasks = 0;
    int quit = 0;
    Progam running_tasks_array[max_parallel_tasks];

    FCFS_Task *fcfs_queue = NULL;

    Progam *args = malloc(sizeof(Progam));
    while (read(fd, args, sizeof(Progam)) > 0)
    {

        if (strcmp(args->mode[0], "execute") == 0)
        {
            if (!quit)
            {
                gettimeofday(&args->tempo_inicio, NULL);
                args->taskid = taskid++;
                if (strcmp(sched_policy, "SJF") == 0)
                {
                    enqueue_sjf(&fcfs_queue, *args);
                }
                else if (strcmp(sched_policy, "LJF") == 0)
                {
                    enqueue_ljf(&fcfs_queue, *args);
                }
                else
                {
                    enqueue(&fcfs_queue, *args);
                }
                char response_fifo[256];
                sprintf(response_fifo, CLIENT "%d", args->pid);
                int fd_response = open(response_fifo, O_WRONLY);
                
                if (fd_response == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                write(fd_response, &args->taskid, sizeof(int));
                close(fd_response);
                if (running_tasks < max_parallel_tasks && fcfs_queue != NULL)
                {
                    Progam task = dequeue(&fcfs_queue);
                    running_tasks_array[running_tasks] = task;
                    if (fork() == 0)
                    {
                        task.pid = getpid();
                        if (strcmp(task.mode[1], "-u") == 0)
                        {
                            task.tempo_exec = mysystem(task.command, task.taskid, argv[1],task.tempo_inicio);
                        }
                        else if (strcmp(task.mode[1], "-p") == 0)
                        {
                            task.tempo_exec = exec_pipes(task.command, task.taskid, argv[1],task.tempo_inicio);
                        }
                        strcpy(task.mode[0], "fork");
                        write(fd_write, &task, sizeof(Progam));
                        close(fd_write);
                        exit(0);
                    }
                    else
                    {
                        running_tasks++;
                    }
                }
            }
            else
            {
                char response_fifo[256];
                sprintf(response_fifo, CLIENT "%d", args->pid);
                int fd_response = open(response_fifo, O_WRONLY);
                
                if (fd_response == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                int res = -1;
                write(fd_response, &res, sizeof(int));
                close(fd_response);
            }
        }
        else if (strcmp(args->mode[0], "fork") == 0)
        {
            int ftask = 0;
            waitpid(args->pid, NULL, 0);
            int fd_out = open(output_file_full, O_WRONLY | O_CREAT, 0666);
            
            if (fd_out == -1)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
            Finished_task tasktowrite = {0};
            tasktowrite.taskid = args->taskid;
            tasktowrite.tempo_exec = args->tempo_exec;
            strcpy(tasktowrite.command, args->command);
            lseek(fd_out, 0, SEEK_END);
            write(fd_out, &tasktowrite, sizeof(Finished_task));
            close(fd_out);
            for (int i = 0; i < running_tasks; i++)
            {
                if (args->taskid == running_tasks_array[i].taskid)
                {
                    ftask = i;
                    break;
                }
            }

            running_tasks--;
            if (running_tasks < max_parallel_tasks && fcfs_queue != NULL)
            {
                Progam task = dequeue(&fcfs_queue);
                running_tasks_array[ftask] = task;
                if (fork() == 0)
                {
                    task.pid = getpid();
                    if (strcmp(task.mode[1], "-u") == 0)
                    {
                        task.tempo_exec = mysystem(task.command, task.taskid, argv[1],task.tempo_inicio);
                    }
                    else if (strcmp(task.mode[1], "-p") == 0)
                    {
                        task.tempo_exec = exec_pipes(task.command, task.taskid, argv[1],task.tempo_inicio);
                    }
                    strcpy(task.mode[0], "fork");
                    write(fd_write, &task, sizeof(Progam));
                    close(fd_write);
                    exit(0);
                }
                else
                {
                    running_tasks++;
                }
            }
        }
        else if (strcmp(args->mode[0], "status") == 0)
        {
            if (fork() == 0)
            {
                char response_fifo_exec[256];
                char response_fifo_sched[256];
                char response_fifo_done[256];
                sprintf(response_fifo_exec, CLIENT "%d_exec", args->pid);
                sprintf(response_fifo_sched, CLIENT "%d_sched", args->pid);
                sprintf(response_fifo_done, CLIENT "%d_done", args->pid);
                int fd_response_exec = open(response_fifo_exec, O_WRONLY);
                if (fd_response_exec == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                int fd_response_sched = open(response_fifo_sched, O_WRONLY);
                if (fd_response_sched == -1)
                {
                    unlink(response_fifo_exec);
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                int fd_response_done = open(response_fifo_done, O_WRONLY);
                if (fd_response_done == -1)
                {
                    unlink(response_fifo_exec);
                    unlink(response_fifo_sched);
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                for (int i = 0; i < running_tasks; i++)
                {
                    write(fd_response_exec, &running_tasks_array[i], sizeof(Progam));
                }
                close(fd_response_exec);
                for (FCFS_Task *current = fcfs_queue; current != NULL; current = current->next)
                {
                    write(fd_response_sched, &current->task, sizeof(Progam));
                }
                close(fd_response_sched);
                int fdout=open(output_file_full, O_RDONLY);
                Finished_task tasktoread;
                while (read(fdout, &tasktoread, sizeof(Finished_task)) > 0)
                {
                    write(fd_response_done, &tasktoread, sizeof(Finished_task));
                }
                close(fdout);
                close(fd_response_done);
                args->pid = getpid();
                strcpy(args->mode[0], "statusf");
                write(fd_write, args, sizeof(Progam));
                close(fd_write);
                exit(0);
            }
        }
        else if (strcmp(args->mode[0], "statusf")==0)
        {
            waitpid(args->pid, NULL, 0);
        }
        else if (strcmp(args->mode[0], "quit") == 0)
        {
            quit = 1;
        }
        if (quit == 1 && running_tasks == 0 && fcfs_queue == NULL)
        {
            free(args);
            close(fd_write);
            close(fd);
            unlink(SERVER);
            return 0;
        }
    }
}
