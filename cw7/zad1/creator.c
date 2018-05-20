#include "shared_memory.h"

int shmid;
Fifo *fifo;
int number_of_clients, number_of_cuts;

void getChair();
void releaseResources();
void takeActionIfBarberWasAwake();
void takeActionIfBarberWasSleeping();
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


int main(int argc, char *argv[]) {
	signal(SIGINT,exitHandler);
	parseArgs(argc,argv);
	if(atexit(releaseResources)!=0) FAILURE_EXIT("Failed to set atexit function\n");
	prepareResources();
	
    
    for(int i=0;i<number_of_clients;i++){
    	pid_t pid = fork();
    	if(pid == 0){
    		for(int i=0;i<number_of_cuts;i++){
			
				modifySemaphore(BARBER_ROOM,-1);
			
				int barber_is_awake = semctl(semid, AWAKE, GETVAL, 0);
				if(barber_is_awake ==1){
					takeActionIfBarberWasAwake();
		 
				}else{
					takeActionIfBarberWasSleeping();
				}
			}
			return 0;
    	}	
    }
    
	while(wait(NULL));
    printf("All clients done they work, end\n");
    return 0;
}


void takeActionIfBarberWasAwake(){

	modifySemaphore(BARBER_ROOM,1);
	modifySemaphore(WAITING_ROOM,-1);
	
	if(isFull(fifo)){
		printf("%ld: The queue is full and I leave: %i\n",getTime(),getpid());
		modifySemaphore(WAITING_ROOM,1);
	}else{
		printf("%ld: I take place in waiting room: %i\n",getTime(),getpid());
		push(fifo,getpid());
		modifySemaphore(WAITING_ROOM,1);
		modifySemaphore(SEND_CHAIR_MSG,-1);
		printf("%ld: I sit on a chair: %i\n",getTime(),getpid());
		modifySemaphore(SEND_OUT_MSG,-1);
		printf("%ld: My chair is cut and I leave: %i\n",getTime(),getpid());
		
		
	//	sigset_t myset;
	//	sigemptyset(&myset);
	//	if(sigsuspend(&myset) == -1) FAILURE_EXIT("Error when setting process signal mask\n");
	}
	
	
}

void takeActionIfBarberWasSleeping(){
	printf("%ld: I wake barber up: %i\n",getTime(),getpid());
	printf("%ld: I sit on a chair: %i\n",getTime(),getpid());
	fifo->chair = getpid();
	modifySemaphore(AWAKE,1);
	modifySemaphore(SEND_OUT_MSG,-1);
	printf("%ld My chair is cut and I leave: %i\n",getTime(),getpid());
	modifySemaphore(BARBER_ROOM,1);
	
	
}





void releaseResources(){
	if(shmdt(fifo) == -1) FAILURE_EXIT("Cannot detach data from shared memory\n"); 
	printf("Creator released resourcess\n");
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
    semid = semget(key, 5,0666);
    if(semid == -1) FAILURE_EXIT("Error when trying to open a semaphore\n");

}




