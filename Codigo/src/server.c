#include "includes.h"
#include "mysystem.h"

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

    char output_file_full[256];
    sprintf(output_file_full, "../%s/output_full.txt", argv[1]);
    char *fifopath = "../tmp/fifo";
    mkfifo(fifopath, 0666);
    int taskid = 1;
    int fd = open(fifopath, O_RDONLY);
    char *sched_policy = argv[3];
    int max_parallel_tasks = atoi(argv[2]);
    int running_tasks = 0;

    FCFS_Task *fcfs_queue = NULL;

    while (1)
    { // Infinite loop until termination command received
        if (running_tasks < max_parallel_tasks && fcfs_queue != NULL)
        {
            Progam task = dequeue(&fcfs_queue);
            if (fork() == 0)
            {
                task.pid = getpid();
                task.tempo_exec= mysystem(task.command, task.taskid, argv[1]);
                strcpy(task.mode[0], "fork");
                int fd_temp = open(fifopath, O_WRONLY);
                write(fd_temp, &task, sizeof(Progam));
                close(fd_temp);
                exit(0);
            }
            else
            {
                running_tasks++;
            }
        }
        Progam *args = malloc(sizeof(Progam));
        if (read(fd, args, sizeof(Progam)) <= 0)
        {
            //free(args);
            continue;
        }

        if (strcmp(args->mode[0], "execute") == 0)
        {
            args->taskid = taskid++;
            if (strcmp(sched_policy, "SJF") == 0)
            {
                enqueue_sjf(&fcfs_queue, *args);
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
            running_tasks--;
        }
        else if (strcmp(args->mode[0], "quit") == 0)
        {
            break; // Termination command received, exit the loop
        }

        free(args);
    }
    free(fcfs_queue);
    close(fd);
    unlink(fifopath);
    return 0;
}
