#include "includes.h"
#include "mysystem.h"

void extractPipe(const char *command, int N, char **commandPipeline) {
    const char *start = command;
    int i = 0;

    while (*start != '\0' && i < N) {
        // Ignorar espaços em branco no início
        while (*start == ' ' || *start == '\n') {
            start++;
        }

        // Encontrar o fim do token
        const char *end = start;
        while (*end != '\0' && *end != '|' && *end != '\n') {
            end++;
        }

        // Alocar memória para o token
        int token_length = end - start;
        commandPipeline[i] = (char *)malloc(token_length + 1);
        if (commandPipeline[i] == NULL) {
            // Lidar com erro de alocação de memória
            exit(EXIT_FAILURE);
        }

        // Copiar o token para o array de processos
        strncpy(commandPipeline[i], start, token_length);
        commandPipeline[i][token_length] = '\0';

        // Atualizar os índices
        i++;
        start = end;
    }
}

long executePipeline(const char *command, int task_id, char *output_folder){
    int N = 0;
    char **commandPipeline = malloc(N * sizeof(char *));
    extractPipe(command, N, commandPipeline);
    int p[N - 1][2];
    for (int i = 0; i < N - 1; i++) {
        pipe(p[i]);
    }

    pid_t pid;
    int fd;
    char output_file[256];
    sprintf(output_file, "../%s/output_task_id_%d.txt", output_folder, task_id);

    for (int i = 0; i < N; i++) {
        pid = fork();
        if (pid == 0) {
            // Se não for o primeiro comando, redirecionar a entrada para o pipe anterior
            if (i > 0) {
                dup2(p[i - 1][0], STDIN_FILENO);
                close(p[i - 1][0]);
                close(p[i - 1][1]);
            }
            // Se não for o último comando, redirecionar a saída para o próximo pipe
            if (i < N - 1) {
                dup2(p[i][1], STDOUT_FILENO);
                close(p[i][0]);
                close(p[i][1]);
            } else {
                // Se for o último comando, redirecionar a saída para o arquivo de saída
                fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // Dividir o comando em argumentos
            char *args[20];
            char *token = strtok(commandPipeline[i], " ");
            int j = 0;
            while (token != NULL) {
                args[j++] = token;
                token = strtok(NULL, " ");
            }
            args[j] = NULL;

            // Executar o comando
            long ress = mysystem(command, task_id, output_folder);
            perror("exec");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Fechar todos os pipes no processo pai
    for (int i = 0; i < N - 1; i++) {
        close(p[i][0]);
        close(p[i][1]);
    }

    // Esperar que todos os filhos terminem
    int status;char *args[20];
    while (wait(&status) > 0);

    return 0; // ou retorne o tempo total, se necessário
}


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
                    if (fork() == 0)
                    {
                        task.pid = getpid();
                        task.tempo_exec = mysystem(task.command, task.taskid, argv[1]);
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
            waitpid(args->pid, NULL, 0);
            char buffer[500];
            sprintf(buffer, "Taskid: %d\nProgama:%s\nTempo execução: %ld\n\n", args->taskid, args->command, args->tempo_exec);
            int fd_out = open(output_file_full, O_WRONLY | O_CREAT, 0666);
            lseek(fd_out, 0, SEEK_END);
            write(fd_out, buffer, strlen(buffer));
            close(fd_out);
            running_tasks--;
            if (running_tasks < max_parallel_tasks && fcfs_queue != NULL)
            {
                Progam task = dequeue(&fcfs_queue);
                if (fork() == 0)
                {
                    task.pid = getpid();
                    if (strcmp(task.mode[1], "-u" )== 0)
                    {
                    task.tempo_exec = mysystem(task.command, task.taskid, argv[1]);
                    }
                    else if (strcmp(task.mode[1], "-p") == 0){
                    task.tempo_exec = executePipeline(task.command, task.taskid, argv[1]);

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