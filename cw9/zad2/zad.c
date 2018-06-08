#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/resource.h>
char buffer[255];
#define _GNU_SOURCE
#define WRITE(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"

#define MAX_BUFFER_SIZE 4096

volatile int end=0;
int L;
int P, K;
int N;
char file_name[40];
int search_mode,verbose,nk;
pthread_t *producer_threads=NULL,*consumer_threads=NULL;
sem_t  *semaphores=NULL;
sem_t emptyCount, fillCount, produce, consume;

pthread_cond_t not_full, not_empty;
FILE *file;
volatile int finish_work=0;


typedef struct{
    char* array[MAX_BUFFER_SIZE];
    int consume_i ,produce_i, nelements;
    
}Buffer;

Buffer buf;

void signalHandler(int signo){
    printf("I received signal %s\n",signo == SIGINT ? "SIGINT" : "SIGALRM");
    exit(0);
}


void * doProducerWork(void * arg){
    
    while(1){
       
        // ------------------- produce --------------------------
        if(verbose)WRITE(MAG"PRODUCER %ld takes line from file\n",pthread_self());
        char * line = malloc(256);
        if(fgets(line, 256, file) == NULL){
            end=1;
            if(verbose)WRITE(MAG"PRODUCER %ld found EOF\n",pthread_self());
            if(nk==0){
                return NULL;
            }else{
                while(1){};
            }
        } 
        //-----------------------end produce------------------------
    

        sem_wait(&emptyCount);
        sem_wait(&semaphores[buf.produce_i]);
        // ------------------put item into buffer ------------------------------
        buf.array[buf.produce_i] = line;
        int previous = buf.produce_i;
        buf.produce_i = (buf.produce_i +1) % N;
        buf.nelements++;
        if(verbose)WRITE(MAG"PRODUCER %ld puts line to buf[%i]\n",pthread_self(),previous);
        // -----------------end put item into buffer-----------------------------
        sem_post(&semaphores[previous]);
        sem_post(&fillCount);
        

    }
}

void * doConsumerWork(void *arg){
    
    while(1){
        
        sem_wait(&fillCount);
        sem_wait(&semaphores[buf.consume_i]);
        // ------------------- remove item from buffer -------------------------------
        char *line = buf.array[buf.consume_i];
        buf.array[buf.consume_i] = NULL;

        int previous = buf.consume_i;
        buf.consume_i = (buf.consume_i +1) % N;
        buf.nelements--;
        if(verbose)WRITE(CYN"CONSUMER %ld freed buf[%i] \n",pthread_self(),previous);

        //-------------------- end remove item from buffer ---------------------------
        sem_post(&semaphores[previous]);
        sem_post(&emptyCount);



       
        //----------------------       consume item ----------------------------------------        
        if(verbose) printf(CYN"CONSUMER %ld consumes buf[%i]\n",pthread_self(),buf.consume_i);
        if(line ==NULL){
            //WRITE("Nothing\n");
           
        }else{
            switch(search_mode){
                case -1:
                    if(strlen(line) < L) WRITE(CYN"CONSUMER %ld found: %s",pthread_self(),line);
                    break; 
                case 0:
                    if(strlen(line) == 0) WRITE(CYN"CONSUMER %ld found: %s",pthread_self(),line); 
                    break;
                case 1:
                    if(strlen(line) > 0) WRITE(CYN"CONSUMER %ld found: %s",pthread_self(),line); 
                    break;
            }
            free(line);
           
        }

        //------------------------------end consume ---------------------------------    
        
    }

}

void printInfo(){
    printf("Please enter correct arguments:\n");
    printf("./zad  <configuration file path>\n");
    printf("Configuration args: P K N file_name search_mode <L> verbose nk\n");
    printf("<search_mode>: -1 lower , 0 equal, 1 greater than <L>\n");
    printf("./zad ../configuration.txt \n");
    exit(0);
}

void parseCommandArgs(int argc, char * argv[]){
    if(argc !=2) printInfo();
    
    FILE *configuration;
    if((configuration = fopen(argv[1],"r")) == NULL) FAILURE_EXIT("Opening configuration file failed\n");

    if(fscanf(configuration,"%i %i %i %s %i %i %i %i",&P,&K,&N,file_name,&search_mode,&L,&verbose,&nk)==-1) FAILURE_EXIT("Error while parsing:%s\n",argv[1]);
    
    if(N >= MAX_BUFFER_SIZE) FAILURE_EXIT("Given buffer size can't be larger than %i",MAX_BUFFER_SIZE);
   
    buf.produce_i = 0;
    buf.consume_i = 0;
    buf.nelements =0;
    for(int i=0;i<N;i++){
        buf.array[i] = NULL;
    }
    if(nk < 0) FAILURE_EXIT("Incorrect nk\n");
    fclose(configuration);
}

void releaseResources(){
    // for(int p=0;p<P;p++){
    //     if(pthread_cancel(producer_threads[p])!=0) printf("Failed to cancel Pthread\n");
    // }
    // for(int k=0;k<K;k++){
    //     if(pthread_cancel(producer_threads[k])!=0) printf("Failed to cancel Cthread\n");
    // }

}


int main(int argc, char * argv[]){
    
    signal(SIGINT,signalHandler);
    signal(SIGALRM,signalHandler);
    
    parseCommandArgs(argc,argv);
    if((file = fopen(file_name,"r")) == NULL) FAILURE_EXIT("Opening: %s failed\n",file_name); 
    
    
    semaphores = (sem_t *) calloc(N,sizeof(sem_t));
    for(int i=0;i<N;i++){
        sem_init(&semaphores[i],0,1);
    }
    sem_init(&emptyCount,0,1);
    sem_init(&fillCount,0,1);
    sem_init(&produce,0,1);
    sem_init(&consume,0,1);
    

    pthread_cond_init(&not_empty,NULL);
    pthread_cond_init(&not_full,NULL);

    producer_threads = calloc(P,sizeof(pthread_t));
    consumer_threads = calloc(K,sizeof(pthread_t));

    if(nk > 0) alarm(nk);
    for(int p=0;p<P;p++){
        pthread_create(&producer_threads[p],NULL,doProducerWork,NULL);
    }
    for(int k=0;k<K;k++){
        pthread_create(&consumer_threads[k],NULL,doConsumerWork,NULL);  
    }
    if(atexit(releaseResources)!=0) FAILURE_EXIT("Failed to set atexit function\n");
   



    for(int p=0;p<P;p++){
        if(pthread_join(producer_threads[p],NULL)!=0) FAILURE_EXIT("Waiting for producer thread failed: %s\n",strerror(errno));
    }
    finish_work=1;
    if(verbose) WRITE("All producents finished their work\n");
    if(verbose) WRITE("All consuments finished their work\n");
    exit(0);
    pthread_cond_broadcast(&not_empty);
    for(int k=0;k<K;k++){
        if(pthread_join(consumer_threads[k],NULL)!=0) FAILURE_EXIT("Waiting for consumer thread failed: %s\n",strerror(errno));
    }
    if(verbose) WRITE("All consuments finished their work\n");

    fclose(file);
}



