
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



#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define W(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define QUEUE_SIZE 4096
#define PROJECT_ID 12
typedef struct{
	pid_t queue[QUEUE_SIZE];
    int head ,tail ,size , nelements ;
    pid_t chair;
}Fifo;

int semid;

int getSize(Fifo * fifo);

int push(Fifo *fifo,int val);

int pop(Fifo * fifo);

void init(Fifo * fifo);

int isEmpty(Fifo * fifo);

int isFull(Fifo * fifo);


struct sembuf sops;
enum SemTypes{BARBER_ROOM =0, WAITING_ROOM=1, AWAKE =2,SEND_CHAIR_MSG=3,SEND_OUT_MSG=4};

void modifySemaphore(int name, int val);
#endif //SHAREDMEMORY_H
