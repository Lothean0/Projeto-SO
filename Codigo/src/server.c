#include "includes.h"
#include "mysystem.h"

void enqueue(FCFS_Task **queue, Progam task) {
    FCFS_Task *new_task = (FCFS_Task *)malloc(sizeof(FCFS_Task));
    new_task->task = task;
    new_task->next = NULL;

    if (*queue == NULL) { // Queue is empty
        *queue = new_task; // Set the new task as the first task in the queue
    } else {
        FCFS_Task *current = *queue; // Traverse the queue to find the last task
        while (current->next != NULL) { // Find the last task in the queue
            current = current->next; // Move to the next task
        }
        current->next = new_task; // Set the new task as the last task in the queue
    }
}

Progam dequeue(FCFS_Task **queue) {
    FCFS_Task *temp = *queue; // Get the first task in the queue
    Progam task = temp->task; // Get the task from the FCFS_Task struct
    *queue = (*queue)->next; // Set the next task as the first task in the queue
    free(temp); // Free the memory allocated for the FCFS_Task struct
    return task; // Return the task
}

int compare_tasks(const void *a, const void *b) { //void pointer because qsort requires a function with this signature
    Progam *task_a = (Progam *)a; 
    Progam *task_b = (Progam *)b;
    return task_a->time - task_b->time;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s output_folder parallel-tasks sched-policy\n", argv[0]);
        return -1;
    }

    char output_file_full[256];
    sprintf(output_file_full, "../%s/output_full.txt", argv[1]);
    char *fifopath = "../tmp/fifo";
    mkfifo(fifopath, 0666);
    int taskid = 1;
    int fd_temp;
    int fd = open(fifopath, O_RDONLY);
    char *sched_policy = argv[3];
    int max_parallel_tasks = atoi(argv[2]);
    int running_tasks = 0;

    FCFS_Task *fcfs_queue = NULL;

    while (1) { // Infinite loop until termination command received
        Progam *args = malloc(sizeof(Progam));
        if (read(fd, args, sizeof(Progam)) <= 0) {
            free(args);
            continue;
        }

        if (strcmp(args->mode[0], "execute") == 0) {
            args->taskid = taskid++;
            enqueue(&fcfs_queue, *args);
            char response_fifo[256];
            sprintf(response_fifo, "../tmp/response_fifo_%d", args->pid);
            int fd_response = open(response_fifo, O_WRONLY);
            write(fd_response, &args->taskid, sizeof(int));
            close(fd_response);
            running_tasks++;

            if (strcmp(sched_policy, "SJF") == 0) { // SJF scheduling
                // Sort the tasks in the queue based on their execution time
                Progam *task_array = malloc(max_parallel_tasks * sizeof(Progam));
                FCFS_Task *current = fcfs_queue;
                int i = 0;
                while (current != NULL && i < max_parallel_tasks) {
                    task_array[i++] = current->task;
                    current = current->next;
                }
                qsort(task_array, i, sizeof(Progam), compare_tasks);

                if (running_tasks <= max_parallel_tasks) {
                    if (fork() == 0) {
                        long tempo_exec = mysystem(task_array[0].command, task_array[0].taskid, argv[1]);
                        args->pid = getpid();
                        strcpy(args->mode[0], "fork");
                        args->tempo_exec = tempo_exec;
                        fd_temp = open(fifopath, O_WRONLY);
                        write(fd_temp, args, sizeof(Progam));
                        close(fd_temp);
                        exit(0);
                    }
                }
                free(task_array);
            }
            else if (strcmp(sched_policy, "FCFS") == 0) { // FCFS scheduling
                if (running_tasks <= max_parallel_tasks) {
                    if (fork() == 0) {
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
            }
        }
        else if (strcmp(args->mode[0], "fork") == 0) {
            waitpid(args->pid, NULL, 0);
            char buffer[500];
            sprintf(buffer, "Taskid: %d\nProgama:%s\nTempo execução: %ld\n\n", args->taskid, args->command, args->tempo_exec);
            int fd_out = open(output_file_full, O_WRONLY | O_CREAT, 0666);
            lseek(fd_out, 0, SEEK_END);
            write(fd_out, buffer, strlen(buffer));
            close(fd_out);
            running_tasks--;
        }
        else if (strcmp(args->mode[0], "quit") == 0) {
            break; // Termination command received, exit the loop
        }

        free(args);
    }

    close(fd);
    unlink(fifopath);
    return 0;
}
