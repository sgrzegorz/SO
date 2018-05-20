#include "shared_memory.h"


int shmid;
Fifo *fifo=NULL;
int fifo_size;

void prepareResources();
void releaseResources();

void parseArgs(int argc, char *argv[]){
	
	if(argc!=2){
		FAILURE_EXIT("./creator <size of queue> \n");
	}
	
	int tmp = atoi(argv[1]);
	if(tmp >QUEUE_SIZE) FAILURE_EXIT("Given size cannot be larger than %i",QUEUE_SIZE);
	fifo_size = tmp;
}

void exitHandler(int signo){
	exit(0);
}


int main(int argc, char*argv[]){
	parseArgs(argc,argv);
	WRITE_MSG("!3\n");
    prepareResources();
    if(atexit(releaseResources) != 0) FAILURE_EXIT("Failed to set atexit function\n");
	signal(SIGINT,exitHandler);
    
	
    if(semctl(semid,SEND_OUT_MSG,SETVAL,1) == -1) FAILURE_EXIT("Failed to set semaphore1\n");
	if(semctl(semid,SEND_CHAIR_MSG,SETVAL,1) == -1) FAILURE_EXIT("Failed to set semaphore2\n");
	if(semctl(semid,AWAKE,SETVAL,1) == -1) FAILURE_EXIT("Failed to set semaphore3\n");

	if(semctl(semid, BARBER_ROOM, SETVAL, 0) == -1) FAILURE_EXIT("Failed to set semaphore4\n");
	if(semctl(semid,WAITING_ROOM,SETVAL,1) == -1) FAILURE_EXIT("Failed to set semaphore5\n");

         
	while(1){
		modifySemaphore(WAITING_ROOM,-1);
		
    	if(isEmpty(fifo)){//In waiting room are no clients
    		modifySemaphore(WAITING_ROOM,1);
    		modifySemaphore(BARBER_ROOM,1);
    		printf("%ld: BARBER: I go to sleep\n",getTime());
    		modifySemaphore(AWAKE,-1);
    		printf("BARBER: I wake up\n");
 
    	}else{//There was a client in waiting room
    		modifySemaphore(WAITING_ROOM,1);
    		pid_t client = pop(fifo);
    		printf("%ld: BARBER: I invite client: %i\n",getTime(),client);
    		modifySemaphore(BARBER_ROOM,1);
    		fifo->chair = getpid();
    		modifySemaphore(SEND_CHAIR_MSG,1);
    	}
    	
    	//Cut client's chair
    	printf("%ld: BARBER: I cut: %i\n",getTime(),fifo->chair);
    	printf("%ld: BARBER: I finished cut: %i\n",getTime(),fifo->chair);
    	modifySemaphore(SEND_OUT_MSG,1);
    	modifySemaphore(BARBER_ROOM,-1);	

    }

}

void prepareResources(){
	char *path = getenv("HOME");
	if(path == NULL) FAILURE_EXIT("Failed to get environment variable\n");
    key_t key = ftok(path,PROJECT_ID);
	if(key == -1) FAILURE_EXIT("Failed to create project key\n");
  

    shmid = shmget(key,sizeof(Fifo),IPC_CREAT|0666);
	
    if(shmid == -1) FAILURE_EXIT("Preparing resources failed\n");

    fifo = shmat(shmid,(void*) 0,0);
    if(fifo == (Fifo*)(-1)) FAILURE_EXIT("Preparing resources failed1\n");

    init(fifo);
 
    fifo->size = fifo_size;
    
  
    semid = semget(key,5,IPC_CREAT|0666);
    if(semid == -1) FAILURE_EXIT("Error while creating semaphore %s\n",strerror(errno));
    
    
}

void releaseResources(){
	if(shmdt(fifo) == -1) printf("Cannot detach data from shared memory\n");
	if(shmctl(shmid, IPC_RMID, 0)==-1) printf("Cannot delete data in shared memory\n");
	if(semctl(semid,0,IPC_RMID) == -1) printf("Deleting semaphores failed %s\n",strerror(errno));
	printf("Barber released resourcess\n");
}

