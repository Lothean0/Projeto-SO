#if !defined(INCLUDES)
#define INCLUDES

#define MAX__PARALLEL_TASKS 1000
#define MAX_LINE_LENGTH 1024
#define MAX_COMMANDS 1000 // Maximum number of commands in the pipeline

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
struct tempo{
	long segundos;
	long microseg;
};
int gettimeofday(struct tempo *restrict tp, void *restrict tzp);
typedef struct Progam
{
pid_t pid;
int taskid;
int argc; 
char mode[2][8]; //execute or status e -u ou -p
int time;
char command[300];
long tempo_exec;
}Progam;

typedef struct FCFS_Task {
    Progam task;
    struct FCFS_Task *next;
} FCFS_Task;
#endif

