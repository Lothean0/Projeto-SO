#if !defined(INCLUDES)
#define INCLUDES

#define MAX_LINE_LENGTH 1024

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
int argc;
char mode[2][8]; //execute or status e -u ou -p
int time;
char command[300];
}Progam;

typedef struct Task	{	
	int taskid;
	char command[300];
}Task;

#endif
