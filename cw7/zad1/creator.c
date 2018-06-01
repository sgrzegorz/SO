#include "shared_memory.h"

int shmid;
Fifo *fifo;
int number_of_clients, number_of_cuts;

void getChair();
void releaseResources();
void takeActionIfBarberIsInBed();
void takeActionIfBarberIsNotInBed();
void prepareResources();



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
	signal(SIGINT,exitHandler);
	signal(SIGRTMIN,handler);
	
	parseArgs(argc,argv);
	if(atexit(releaseResources)!=0) FAILURE_EXIT("Failed to set atexit function\n");
	prepareResources();
	
    if(semctl(semid,CLIENTS_BLOCADE,SETVAL,0) == -1) FAILURE_EXIT("Failed to set semaphore2\n");
	
	
    for(int i=0;i<number_of_clients;i++){
    	pid_t pid = fork();
    	if(pid == 0){
			
    		for(int i=0;i<number_of_cuts;i++){
				
				
				sops[0].sem_num = CLIENTS_BLOCADE;
    			sops[0].sem_op = 0;
				sops[1].sem_num = BED_QUEUE_BLOCADE;
    			sops[1].sem_op = -1;
				if(semop(semid,&sops[0],2) == -1) FAILURE_EXIT("Failed to change semaphores unlock \n");

				// modifySemaphore(CLIENTS_BLOCADE,0);
				
				// modifySemaphore(BED_QUEUE_BLOCADE,-1);
				
				printf("%d\n",fifo->barber_in_bed);
				if(fifo->barber_in_bed){
					takeActionIfBarberIsInBed();
				}else{
					takeActionIfBarberIsNotInBed();
				}

				
			}
			exit(0);
    	}	
    }
    
	while(wait(NULL));
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
	modifySemaphore(BED_QUEUE_BLOCADE,1);

	kill(fifo->barber_pid,SIGRTMIN);
	sigsuspend(&mask);

	printf(MAG"%ld: My chair is cut and I leave: %i\n",getTime(fifo),getpid());
	
}

void takeActionIfBarberIsNotInBed(){

	if(isFull(fifo)){
		printf(BLU"%ld: The queue is full and I leave: %i\n",getTime(fifo),getpid());
		modifySemaphore(BED_QUEUE_BLOCADE,1);
	}else{
		printf(BLU "%ld: I take place in waiting room: %i\n",getTime(fifo),getpid());
		push(fifo,getpid());
		modifySemaphore(BED_QUEUE_BLOCADE,1);
		sigset_t mask;
		sigemptyset(&mask);
		sigsuspend(&mask);

		printf(GRN"%ld: I sit on a chair: %i\n",getTime(fifo),getpid());
		kill(fifo->barber_pid,SIGRTMIN);
		sigsuspend(&mask);

		printf(MAG"%ld: My chair is cut and I leave: %i\n",getTime(fifo),getpid());
		kill(fifo->barber_pid,SIGRTMIN);
		
	//	sigset_t myset;
	//	sigemptyset(&myset);
	//	if(sigsuspend(&myset) == -1) FAILURE_EXIT("Error when setting process signal mask\n");
	}
}

void haveChairCut(){

}



void releaseResources(){
	if(shmdt(fifo) == -1) FAILURE_EXIT("Cannot detach data from shared memory\n"); 
	// printf(BLU"Creator released resourcess: %i\n",getpid());
}

void prepareResources(){
	char *path = getenv("HOME");
	if(path == NULL) FAILURE_EXIT("Failed to get environment variable\n");
    key_t key = ftok(path,PROJECT_ID);
	if(key == -1) FAILURE_EXIT("Failed to create project key\n");
	
    shmid = shmget(key, sizeof(Fifo), 0666);
    if(shmid == -1) FAILURE_EXIT("Cannot connect to shared memory\n");
    fifo = shmat(shmid,(void*) 0,0);
    if(fifo== (Fifo*)(-1)) FAILURE_EXIT("Cannot connect to shared memory1\n");
    semid = semget(key, 0,0);
    if(semid == -1) FAILURE_EXIT("Error when trying to open a semaphore\n");

}




