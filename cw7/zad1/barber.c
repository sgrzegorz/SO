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
	
    prepareResources();
    if(atexit(releaseResources) != 0) FAILURE_EXIT("Failed to set atexit function\n");
	signal(SIGINT,exitHandler);
  	 signal(SIGTERM,exitHandler);
	
    if(semctl(semid,BED_QUEUE_BLOCADE,SETVAL,0) == -1) FAILURE_EXIT("Failed to set semaphore1\n");
	if(semctl(semid,CLIENTS_BLOCADE,SETVAL,0) == -1) FAILURE_EXIT("Failed to set semaphore2\n");
	
	while(1){
		
		
    	if(isEmpty(fifo)){//In waiting room are no clients
			
			modifySemaphore(BED_QUEUE_BLOCADE,1);
    		
			fifo->barber_in_bed =0;
    		printf("%ld: BARBER: I go to sleep\n",getTime(fifo));
			modifySemaphore(CLIENTS_BLOCADE,1);
    		while(fifo->barber_in_bed==0){

			}

    		printf("%ld: BARBER: I wake up\n",getTime(fifo));
			kill(SIGRTMIN,fifo->chair);
			sigset_t mask;
			sigemptyset(&mask);
			sigsuspend(&mask);

			printf("%ld: BARBER: I cut: %i\n",getTime(fifo),fifo->chair);
    		printf("%ld: BARBER: I finished cut: %i\n",getTime(fifo),fifo->chair);
			kill(SIGRTMIN,fifo->chair);

 
    	}else{//There was a client in waiting room
    		
    		pid_t client = pop(fifo);
    		printf("%ld: BARBER: I invite client: %i\n",getTime(fifo),client);
    		fifo->chair = getpid();
			modifySemaphore(BED_QUEUE_BLOCADE,1);
			modifySemaphore(CLIENTS_BLOCADE,1);
			kill(SIGRTMIN,fifo->chair);
			sigset_t mask;
			sigemptyset(&mask);
			sigsuspend(&mask);
				
			printf("%ld: BARBER: I cut: %i\n",getTime(fifo),fifo->chair);
    		printf("%ld: BARBER: I finished cut: %i\n",getTime(fifo),fifo->chair);
			kill(SIGRTMIN,fifo->chair);

    	}
    	
    	modifySemaphore(BED_QUEUE_BLOCADE,-1);	

    }

}

void prepareResources(){
	char *path = getenv("HOME");
	if(path == NULL) FAILURE_EXIT("Failed to get environment variable\n");
    key_t key = ftok(path,PROJECT_ID);
	if(key == -1) FAILURE_EXIT("Failed to create project key\n");
  

    shmid = shmget(key,sizeof(Fifo),IPC_CREAT|0666);
    if(shmid == -1) FAILURE_EXIT("Preparing resources failed: %s\n",strerror(errno));

    fifo = shmat(shmid,(void*) 0,0);
    if(fifo == (Fifo*)(-1)) FAILURE_EXIT("Preparing resources failed1\n");

   
    init(fifo);
	fifo->start_time=0;
	fifo->start_time = getTime(fifo);
    fifo->size = fifo_size;
    fifo->barber_pid = getpid();
   
  
    semid = semget(key,2,IPC_CREAT|0666);
	   
    if(semid == -1) FAILURE_EXIT("Error while creating semaphore %s\n",strerror(errno));
    
    
}

void releaseResources(){
	if(shmdt(fifo) == -1) printf("Cannot detach data from shared memory\n");
	if(shmctl(shmid, IPC_RMID, 0)==-1) printf("Cannot delete data in shared memory\n");
	if(semctl(semid,0,IPC_RMID) == -1) printf("Deleting semaphores failed %s\n",strerror(errno));
	printf("Barber released resourcess\n");
}

