#include "shared_memory.h"


void prepareResources();


int main(int argc, char*argv[]){	
    prepareResources();
    
	
	
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

		// sigsuspend(&mask);
		
		fifo->barber_in_cabinet =0;
		while(fifo->client_inside_blocade == 1){}
    }

}


void exitHandler(int signo){
	exit(0);
}
void handler(int signo){}


int parseArgs(int argc, char *argv[]){
	
	if(argc!=2){
		FAILURE_EXIT("./barber <size of queue> \n");
	}
	
	int fifo_size = atoi(argv[1]);
	if(fifo_size >QUEUE_SIZE) FAILURE_EXIT("Given size cannot be larger than %i",QUEUE_SIZE);
	return fifo_size;
}

void releaseResources(){
	if(munmap(fifo,sizeof(Fifo)) == -1) printf("Cannot delete the mapping from process address space\n");

	if(shm_unlink("/shared") == -1) printf("Remove shared memory segment\n");

	if(sem_close(semaphore) == -1) printf("Failed to close named semaphore\n");

	printf("Barber released resourcess\n");
}


void prepareResources(){
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGRTMIN);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	signal(SIGINT,exitHandler);
  	signal(SIGTERM,exitHandler);
	signal(SIGRTMIN,handler);
	
	int fd = shm_open("/shared", O_RDWR, O_CREAT|O_EXCL);
	if(fd == -1) FAILURE_EXIT("Creating shared memory failed\n");

	if(ftruncate(fd,sizeof(Fifo)) == -1) FAILURE_EXIT("Giving shared memory size failed\n");
	
	fifo = mmap(NULL,sizeof(Fifo),PROT_READ|PROT_WRITE,MAP_SHARED)
	if(fifo == (Fifo*)(-1)) FAILURE_EXIT("Failed to create a new mapping in virtual adress space\n");

	semaphore = sem_open("/semaphore",O_CREAT|O_EXCL,O_RDWR,1);
	if(semaphore == SEM_FAILED) FAILURE_EXIT("Creating POSIX semaphore failed\n");
   
    init(fifo);
	fifo->start_time=0;
	fifo->start_time = getTime(fifo);
    fifo->size = parseArgs(argc,argv);;
    fifo->barber_pid = getpid();
   
  
    semid = semget("/semaphore",2,O_CREAT|O_EXCL,O_RDWR,1);
    if(semid == SEM_FAILED) FAILURE_EXIT("Error while creating semaphore %s\n",strerror(errno));
	
	if(atexit(releaseResources) != 0) FAILURE_EXIT("Failed to set atexit function\n");
    
}

