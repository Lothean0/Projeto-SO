#include "includes.h"
#include "mysystem.h"

int main(){
    int fd;
    char * fifopath="fifos";
    int fifo=mkfifo("fifos", 0666);
    return fifo;
}