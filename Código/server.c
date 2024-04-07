#include "includes.h"
#include "mysystem.h"

int main(){
    int fd;
    char * fifopath="fifos/fifo";
    int fifo=mkfifo(fifopath, 0666);
    return fifo;
}