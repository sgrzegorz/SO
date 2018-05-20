#include "shared_memory.h"

int getSize(Fifo * fifo){
    return fifo ->nelements;
}

int push(Fifo *fifo,int val){
    if(getSize(fifo) == QUEUE_SIZE) return -1;
    fifo -> tail = (fifo->tail +1) % fifo->size;
    fifo ->nelements++;
    fifo->queue[fifo->tail] = val;
}

int pop(Fifo * fifo){
    if(getSize(fifo) == 0) return -1;
    int val = fifo->queue[fifo->head];
    fifo->head = (fifo->head +1) % fifo->size;
    fifo-> nelements--;
    return val;
}

void init(Fifo * fifo){
	fifo->size =-1;
 	
    fifo->head = fifo->tail = fifo->nelements =0;
    
   
}

int isEmpty(Fifo * fifo){
    if(getSize(fifo)== 0){
        return 1;
    }else{
        return 0;
    }
}

int isFull(Fifo * fifo){
    if(getSize(fifo)== QUEUE_SIZE){
        return 1;
    }else{
        return 0;
    }    
}

void modifySemaphore(int name, int val){
	sops.sem_num = name;
    sops.sem_op = val;
    
    char *semname;
    switch(name){
    	case AWAKE:
    		semname = "AWAKE";
    		break;
    	case BARBER_ROOM:
    		semname = "BARBER_ROOM";
    		break;
    	case WAITING_ROOM:
    		semname = "WAITING_ROOM";
    		break;
    }
	if(val >= 0){
		
		if(semop(semid,&sops,1) == -1) FAILURE_EXIT("Failed to unlock %s\n",semname);
	}else{
		if(semop(semid,&sops,1) == -1) FAILURE_EXIT("Failed to lock %s\n",semname);
	}
}

// long int getTime(Fifo *fifo){
//     struct timespec time;
//     if(clock_gettime(CLOCK_MONOTONIC,&time)== -1) FAILURE_EXIT("Getting time failed %s\n",strerror(errno));
//     return (long int) (time.tv_sec *1000000 + time.tv_nsec/1000 - fifo->start_time ); 
// }


long int getTime(Fifo *fifo){
    fifo->start_time++;
    return fifo->start_time;
}