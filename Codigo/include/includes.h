#if !defined(INCLUDES)
#define INCLUDES

#define MAX__PARALLEL_TASKS 1000
#define MAX_LINE_LENGTH 1024
#define SERVER "../tmp/fifo"
#define CLIENT "../tmp/response_fifo_"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

struct tempo
{
    long segundos;
    long microseg;
};
int gettimeofday(struct tempo *restrict tp, void *restrict tzp);
typedef struct Progam
{
    pid_t pid;
    int taskid;
    int argc;
    char mode[2][8]; 
    int time;
    char command[300];
    long tempo_exec;
    struct tempo tempo_inicio;
} Progam;

typedef struct Finished_task
{
    int taskid;
    char command[300];
    long tempo_exec;
} Finished_task;

typedef struct FCFS_Task
{
    Progam task;
    struct FCFS_Task *next;
} FCFS_Task;
#endif
