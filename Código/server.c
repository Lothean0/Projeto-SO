#include "includes.h"
#include "mysystem.h"

int main(){
    int fd;
    char * fifopath="fifo";
    int fifo=mkfifo("fifo", 0666);
    return fifo;
}