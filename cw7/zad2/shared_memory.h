
#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H



#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>


#include <sys/sem.h>
#include <unistd.h>


#include <sys/sem.h>

#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/mman.h>
    




#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define QUEUE_SIZE 4096

typedef struct{
	pid_t queue[QUEUE_SIZE];
    int head ,tail ,size , nelements ;
    pid_t chair;
    long int start_time; 
    volatile int barber_in_bed;
    pid_t barber_pid;
    volatile int client_inside_blocade;
    volatile int barber_in_cabinet;
}Fifo;


sem_t *semaphore;
Fifo *fifo;
int shm_fd;


int push(Fifo *fifo,int val);

int pop(Fifo * fifo);

void init(Fifo * fifo);

long getTime();



#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#endif //SHAREDMEMORY_H

