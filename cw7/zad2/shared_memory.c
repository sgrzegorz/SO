#include "shared_memory.h"


int push(Fifo *fifo,int val){
    if(fifo ->nelements == QUEUE_SIZE) return -1; // It'll be error of implementation
    if(fifo ->nelements == fifo->size) return 0; //no place to push 
    fifo -> tail = (fifo->tail +1) % fifo->size;
    fifo->queue[fifo->tail] = val;
    fifo ->nelements++;
}

int pop(Fifo * fifo){
    if(fifo ->nelements == 0) return 0; // no element to pop
    fifo->head = (fifo->head +1) % fifo->size;
    int val = fifo->queue[fifo->head];
    fifo->queue[fifo->head] = -1;
    fifo-> nelements--;
    return val;
}

void init(Fifo * fifo){
	fifo->size =-1;
    fifo->head = fifo->tail = fifo->nelements =0;
    for(int i=0;i<QUEUE_SIZE;i++){
        fifo->queue[i]=-1;
    }
   
}

long int getTime(Fifo *fifo){
    struct timespec time;
    if(clock_gettime(CLOCK_MONOTONIC,&time)== -1) FAILURE_EXIT("Getting time failed %s\n",strerror(errno));
    return (long int) (time.tv_sec *1000000 + time.tv_nsec/1000 - fifo->start_time ); 
}


// long int getTime(Fifo *fifo){
//     fifo->start_time++;
//     return fifo->start_time;
// }