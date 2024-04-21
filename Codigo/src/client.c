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
        if (strcmp(argv[3], "-u") == 0){
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
            strcat(args->command, " "); //
            strcat(args->command, argv[i]);
        }
        // printf("argc: %d\n", args->argc);
        // printf("mode[0]: %s\n", args->mode[0]);
        // printf("mode[1]: %s\n", args->mode[1]);
        // printf("time: %d\n", args->time);
        // printf("args: %s\n", args->command);

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
        unlink(response_fifo);
        }
        else if (strcmp(argv[3] , "-p") == 0){
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
            int N = atoi(argv[4]);
            int pipes[N-1][2];
            for (int i = 0; i < N - 1; i++){
                if (pipe(pipes[i]) == -1){
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }
            for (int i = 0; i < N; i++){
                pid_t child_pid = fork();
                if (child_pid == -1){
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                 else if (child_pid == 0){ 
                    if (i == 0){ //primeiro comando
                    close(pipes[i][0]); //fechar a leitura
                    dup2(pipes[i][1], STDOUT_FILENO); //redirecionar a saida para o pipe
                    close(pipes[i][1]);  //fechar a escrita
                }
                else if (i == N -1){ //ultimo comando
                    close(pipes[i-1][1]);  //fechar a escrita
                    dup2(pipes[i-1][0], STDIN_FILENO); //redirecionar a entrada para o pipe
                    close(pipes[i-1][0]); 
                }
                else { //comandos intermedios
                    close(pipes[i-1][1]);  //fechar a escrita
                    dup2(pipes[i-1][0], STDIN_FILENO); //redirecionar a entrada para o pipe
                    close(pipes[i-1][0]);
                    close(pipes[i][0]);
                    dup2(pipes[i][1], STDOUT_FILENO);
                    close(pipes[i][1]);
                }
                execl(argv[5 + i], argv[5 + i], NULL);
                perror("execl");
                exit(EXIT_FAILURE);
                }
            }
            for (int i = 0; i < N - 1; i++){
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            for (int i = 0; i < N; i++){
                wait(NULL);
            }
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
        unlink(response_fifo);
        }
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
        args->tempo_exec = 0;
        write(fd, args, sizeof(Progam));
        close(fd);
        free(args);
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
        // printf("mode[0]: %s\n", args->mode[0]);
        args->tempo_exec = 0;
        write(fd, args, sizeof(Progam));
        close(fd);
        free(args);
    }
    unlink(response_fifo);
    return 0;
    }
