
#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H



#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define W(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define QUEUE_SIZE 4096
#define PROJECT_ID 12
typedef struct{
	pid_t queue[QUEUE_SIZE];
    int head ,tail ,size , nelements ;
    pid_t chair;
    long int start_time; 
    volatile int barber_in_bed;
    pid_t barber_pid;
    
}Fifo;

union semun  {
    int val;
    struct semid_ds *buf;
    ushort *array;
} arg;




int semid;

int getSize(Fifo * fifo);

int push(Fifo *fifo,int val);

int pop(Fifo * fifo);

void init(Fifo * fifo);

int isEmpty(Fifo * fifo);

int isFull(Fifo * fifo);
long getTime();

struct sembuf sops[10];
enum SemTypes{CLIENTS_BLOCADE =0, BED_QUEUE_BLOCADE=1};

void modifySemaphore(int name, int val);

#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#endif //SHAREDMEMORY_H

