#include "shared_memory.h"


int shmid;
Fifo *fifo=NULL;
int fifo_size;

void prepareResources();
void releaseResources();

void parseArgs(int argc, char *argv[]){
	
	if(argc!=2){
		FAILURE_EXIT("./barber <size of queue> \n");
	}
	
	int tmp = atoi(argv[1]);
	if(tmp >QUEUE_SIZE) FAILURE_EXIT("Given size cannot be larger than %i",QUEUE_SIZE);
	fifo_size = tmp;
}

void exitHandler(int signo){
	exit(0);
}
void handler(int signo){}

int main(int argc, char*argv[]){
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGRTMIN);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	parseArgs(argc,argv);
	
    prepareResources();
    if(atexit(releaseResources) != 0) FAILURE_EXIT("Failed to set atexit function\n");
	signal(SIGINT,exitHandler);
  	signal(SIGTERM,exitHandler);
	signal(SIGRTMIN,handler);
	
	
	while(1){
		
		
    	if(fifo->chair = pop(fifo)){
			printf("\n");
			printf(BLU"%ld: BARBER: I invite client: %i\n",getTime(fifo),fifo->chair);
    		
			fifo->barber_in_cabinet =1;

    	}else{
    					
			fifo->barber_in_bed =1;
    		printf(RED"%ld: BARBER: I go to sleep\n",getTime(fifo));
			
			fifo->barber_in_cabinet =1;
    		while(fifo->barber_in_bed==1){}
			printf("\n");
    		printf(RED"%ld: BARBER: I wake up\n",getTime(fifo));	
    		

    	}

    	sigset_t mask;
		sigemptyset(&mask);
		// printf("%i\n",fifo->chair);
		kill(fifo->chair,SIGRTMIN); //tell him to sit on a chair
		

		sigsuspend(&mask);
	
		printf(MAG"%ld: BARBER: I cut: %i\n",getTime(fifo),fifo->chair);
		printf(MAG"%ld: BARBER: I finished cut: %i\n",getTime(fifo),fifo->chair);
		kill(fifo->chair,SIGRTMIN);

		sigsuspend(&mask);
		
		fifo->barber_in_cabinet =0;
		while(fifo->client_inside_blocade == 1){}
    }

}

void prepareResources(){
	char *path = getenv("HOME");
	if(path == NULL) FAILURE_EXIT("Failed to get environment variable\n");
    key_t key = ftok(path,PROJECT_ID);
	if(key == -1) FAILURE_EXIT("Failed to create project key\n");
	

    shmid = shmget(key,sizeof(Fifo),IPC_CREAT|IPC_EXCL|0666);
    if(shmid == -1) FAILURE_EXIT("Preparing resources failed: %s\n",strerror(errno));

    fifo = shmat(shmid,(void*) 0,0);
    if(fifo == (Fifo*)(-1)) FAILURE_EXIT("Preparing resources failed1\n");

   
    init(fifo);
	fifo->start_time=0;
	fifo->start_time = getTime(fifo);
    fifo->size = fifo_size;
    fifo->barber_pid = getpid();
   
  
    semid = semget(key,2,IPC_CREAT|IPC_EXCL|0666);
	   
    if(semid == -1) FAILURE_EXIT("Error while creating semaphore %s\n",strerror(errno));
    
    
}

void releaseResources(){
	if(shmdt(fifo) == -1) printf("Cannot detach data from shared memory\n");
	if(shmctl(shmid, IPC_RMID, 0)==-1) printf("Cannot delete data in shared memory\n");
	if(semctl(semid,0,IPC_RMID) == -1) printf("Deleting semaphores failed %s\n",strerror(errno));
	printf("Barber released resourcess\n");
}

