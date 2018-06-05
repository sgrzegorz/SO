#include "shared_memory.h"

int number_of_clients, number_of_cuts;

void getChair();
void releaseResources();
void takeActionIfBarberIsInBed();
void takeActionIfBarberIsNotInBed();
void prepareResources();
void finishCutting();



void parseArgs(int argc, char *argv[]){
	if(argc!=3){
		FAILURE_EXIT("./creator <number of clients> <number of cuts per client>\n");
	}
	number_of_clients  = atoi(argv[1]);
	number_of_cuts = atoi(argv[2]);
}

void exitHandler(int signo){
	exit(0);
}
void handler(int signo){}

int main(int argc, char *argv[]) {
	prepareResources(argc,argv);
	
	
    
	
    for(int i=0;i<number_of_clients;i++){
    	pid_t pid = fork();
    	if(pid == 0){
			
    		for(int i=0;i<number_of_cuts;i++){
				sem_wait(semaphore);
				
				while(fifo->barber_in_cabinet==0){}
				fifo->client_inside_blocade=1;


				if(fifo->barber_in_bed){ //takeActionIfBarberIsInBed
					takeActionIfBarberIsInBed();
				}else{ //takeActionIfBarberIsNotInBed
					takeActionIfBarberIsNotInBed();	
				}

				
			}
			
			exit(0);
    	}	
    }
    
	while(wait(NULL)>0);
	int val=-9999;
	if(sem_getvalue(semaphore,&val) == -1) FAILURE_EXIT("Getting semaphore value failed\n");
	if(val == 0) sem_post(semaphore);

    printf("All clients done they work, end\n");
    return 0;
}



void takeActionIfBarberIsInBed(){

	printf(RED"%ld: I wake barber up: %i\n",getTime(fifo),getpid());
	fifo->barber_in_bed =0;
	fifo->chair = getpid();
	sigset_t mask;
	sigemptyset(&mask);

	sigsuspend(&mask);
	printf(GRN"%ld:I sit on a chair: %i\n",getTime(fifo),getpid());
	fifo->client_inside_blocade=0;
	sem_post(semaphore);
	kill(fifo->barber_pid,SIGRTMIN);

	sigsuspend(&mask);
	printf(MAG"%ld: My chair is cut and I leave: %i\n",getTime(fifo),getpid());
	// kill(fifo->barber_pid,SIGRTMIN);
}

void takeActionIfBarberIsNotInBed(){
	if(push(fifo,getpid())){
		
		printf(BLU "%ld: I take place in waiting room: %i\n",getTime(fifo),getpid());
		fifo->client_inside_blocade=0;
		sem_post(semaphore);
		sigset_t mask;
		sigemptyset(&mask);

		sigsuspend(&mask);
		printf("\n");
		printf(GRN"%ld: I sit on a chair: %i\n",getTime(fifo),getpid());
		kill(fifo->barber_pid,SIGRTMIN);

		sigsuspend(&mask);
		printf(MAG"%ld: My chair is cut and I leave: %i\n",getTime(fifo),getpid());
		// kill(fifo->barber_pid,SIGRTMIN);

	}else{
		printf(BLU"%ld: The queue is full and I leave: %i\n",getTime(fifo),getpid());
		fifo->client_inside_blocade=0;
		sem_post(semaphore);
	}
	
}



void releaseResources(){
	if(munmap(fifo,sizeof(Fifo) == -1)) printf("Failed to delete mapping in virtual address space of the process\n");
	if(sem_close(semaphore) == -1) printf("Failed to close semaphore\n");
}

void prepareResources(int argc, char*argv[]){
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGRTMIN);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	signal(SIGINT,exitHandler);
	signal(SIGRTMIN,handler);
	
	parseArgs(argc,argv);
	if(atexit(releaseResources)!=0) FAILURE_EXIT("Failed to set atexit function\n");

	semaphore = sem_open("/semaphore",O_RDWR);
	if(semaphore == SEM_FAILED) FAILURE_EXIT("Failed to open existing semaphore\n");



	shm_fd = shm_open("/shared",O_RDWR,0);
	if(shm_fd ==-1) FAILURE_EXIT("Opening an existing POSIX shared memory object failed");

	fifo = mmap(NULL,sizeof(Fifo),PROT_READ|PROT_WRITE,MAP_SHARED,shm_fd,0);
	if(fifo == MAP_FAILED) FAILURE_EXIT("Create new mapping in virtual address space of the process failed\n");


}




