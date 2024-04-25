#include "includes.h"
#include "mysystem.h"
#include "exec_pipes.h"

void enqueue(FCFS_Task **queue, Progam task)
{
    FCFS_Task *new_task = (FCFS_Task *)malloc(sizeof(FCFS_Task));
    new_task->task = task;
    new_task->next = NULL;

    if (*queue == NULL)
    {                      // Queue is empty
        *queue = new_task; // Set the new task as the first task in the queue
    }
    else
    {
        FCFS_Task *current = *queue; // Traverse the queue to find the last task
        while (current->next != NULL)
        {                            // Find the last task in the queue
            current = current->next; // Move to the next task
        }
        current->next = new_task; // Set the new task as the last task in the queue
    }
}
void enqueue_sjf(FCFS_Task **queue, Progam task)
{
    FCFS_Task *new_task = (FCFS_Task *)malloc(sizeof(FCFS_Task));
    new_task->task = task;
    new_task->next = NULL;

    if (*queue == NULL)
    {                      // Queue is empty
        *queue = new_task; // Set the new task as the first task in the queue
    }
    else
    {
        FCFS_Task *current = *queue;
        FCFS_Task *prev = NULL;
        while (current != NULL && current->task.time < task.time)
        { // Find the correct position for the new task
            prev = current;
            current = current->next;
        }
        if (prev == NULL)
        { // The new task has the shortest execution time
            new_task->next = *queue;
            *queue = new_task;
        }
        else
        { // Insert the new task between prev and current
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
    {                      // Queue is empty
        *queue = new_task; // Set the new task as the first task in the queue
    }
    else
    {
        FCFS_Task *current = *queue;
        FCFS_Task *prev = NULL;
        while (current != NULL && current->task.time > task.time)
        { // Find the correct position for the new task
            prev = current;
            current = current->next;
        }
        if (prev == NULL)
        { // The new task has the shortest execution time
            new_task->next = *queue;
            *queue = new_task;
        }
        else
        { // Insert the new task between prev and current
            prev->next = new_task;
            new_task->next = current;
        }
    }
}

Progam dequeue(FCFS_Task **queue)
{
    FCFS_Task *temp = *queue; // Get the first task in the queue
    Progam task = temp->task; // Get the task from the FCFS_Task struct
    *queue = (*queue)->next;  // Set the next task as the first task in the queue
    free(temp);               // Free the memory allocated for the FCFS_Task struct
    return task;              // Return the task
}

int compare_tasks(const void *a, const void *b)
{ // void pointer because qsort requires a function with this signature
    Progam *task_a = (Progam *)a;
    Progam *task_b = (Progam *)b;
    return task_a->time - task_b->time;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s output_folder parallel-tasks sched-policy\n", argv[0]);
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
    sprintf(output_file_full, "../%s/output_full.txt", argv[1]);
    char fifopath[18] = "../tmp/fifo";
    mkfifo(fifopath, 0666);
    int taskid = 1;
    int fd = open(fifopath, O_RDONLY);
    int fd_write = open(fifopath, O_WRONLY);
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
                sprintf(response_fifo, "../tmp/response_fifo_%d", args->pid);
                int fd_response = open(response_fifo, O_WRONLY);
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
                            task.tempo_exec = mysystem(task.command, task.taskid, argv[1]);
                        }
                        else if (strcmp(task.mode[1], "-p") == 0)
                        {
                            task.tempo_exec = exec_pipes(task.command, task.taskid, argv[1]);
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
                sprintf(response_fifo, "../tmp/response_fifo_%d", args->pid);
                int fd_response = open(response_fifo, O_WRONLY);
                int res = -1;
                write(fd_response, &res, sizeof(int));
                close(fd_response);
            }
        }
        else if (strcmp(args->mode[0], "fork") == 0)
        {
            int ftask = 0;
            waitpid(args->pid, NULL, 0);
            char buffer[500];
            sprintf(buffer, "%d %s %ld ms\n", args->taskid, args->command, args->tempo_exec);
            int fd_out = open(output_file_full, O_WRONLY | O_CREAT, 0666);
            lseek(fd_out, 0, SEEK_END);
            write(fd_out, buffer, strlen(buffer));
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
                        task.tempo_exec = mysystem(task.command, task.taskid, argv[1]);
                    }
                    else if (strcmp(task.mode[1], "-p") == 0)
                    {
                        task.tempo_exec = exec_pipes(task.command, task.taskid, argv[1]);
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
            char response_fifo_exec[256];
            char response_fifo_sched[256];
            char response_fifo_done[256];
            sprintf(response_fifo_exec, "../tmp/response_fifo_%d_exec", args->pid);
            sprintf(response_fifo_sched, "../tmp/response_fifo_%d_sched", args->pid);
            sprintf(response_fifo_done, "../tmp/response_fifo_%d_done", args->pid);
            int fd_response_exec = open(response_fifo_exec, O_WRONLY);
            if (fd_response_exec == -1)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
            int fd_response_sched = open(response_fifo_sched, O_WRONLY);
            if (fd_response_sched == -1)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
            int fd_response_done = open(response_fifo_done, O_WRONLY);
            if (fd_response_done == -1)
            {
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
            write(fd_response_done, output_file_full, strlen(output_file_full) + 1);
            close(fd_response_done);
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
            unlink(fifopath);
            return 0;
        }
    }
}
