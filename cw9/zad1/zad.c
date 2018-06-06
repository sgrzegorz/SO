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
#include <sys/resource.h>

#define _GNU_SOURCE
#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"

#define MAX_BUFFER_SIZE 4096

int L;
int P, K;
int N;
char file_name[40];
int search_mode,verbose,nk;
pthread_t *producer_threads,*consumer_threads;
pthread_mutex_t *mutexes;
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
        pthread_mutex_lock(&mutexes[buf.produce_i]);
        while(buf.nelements == N){
            pthread_cond_wait(&not_full,&mutexes[buf.produce_i]);
        }
        printf("1\n");
        // ------------------- produce -------------------    
        
        
        char * line = malloc(4096);
        if(fgets(line, 4096, file) == NULL) return NULL;
        if(verbose) printf(">>> %s\n",line);
        buf.array[buf.produce_i] = line;
        int previous = buf.produce_i;
        buf.produce_i = (buf.produce_i +1) % N;
        buf.nelements++;
         
        // --------------------------------------------------
        printf("2\n");
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutexes[previous]);
    }
}

void * doConsumerWork(void *arg){
    
    while(1){
        pthread_mutex_lock(&mutexes[buf.produce_i]);
        
        while(buf.nelements == 0){
            pthread_cond_wait(&not_empty,&mutexes[buf.produce_i]);
            if(finish_work){
                pthread_mutex_unlock(&mutexes[buf.produce_i]);
                return NULL; 
            } 
        }
        printf("3\n");
        // ------------------- consume ---------------
        char* line =  buf.array[buf.consume_i];
        buf.array[buf.consume_i] = NULL;

        switch(search_mode){
            case -1:
                if(strlen(line) < L) printf(MAG "%i: %s",buf.consume_i,line);
                break; 
            case 0:
                if(strlen(line) == 0) printf(MAG "%i: %s",buf.consume_i,line); 
                break;
            case 1:
                if(strlen(line) > 0) printf(MAG "%i: %s",buf.consume_i,line); 
                break;
        }
        free(line);
        
        int previous = buf.consume_i;
        buf.consume_i = (buf.consume_i +1) % N;
        buf.nelements--;
        // -------------------------------------------

        printf("4\n");

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutexes[previous]);
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

    fclose(configuration);
}

void releaseResources(){
    for(int p=0;p<P;p++){
        pthread_cancel(producer_threads[p]);
    }
    for(int k=0;k<K;k++){
        pthread_cancel(producer_threads[k]);
    }

}


int main(int argc, char * argv[]){
    
    signal(SIGINT,signalHandler);
    signal(SIGALRM,signalHandler);
    
    parseCommandArgs(argc,argv);
    if((file = fopen(file_name,"r")) == NULL) FAILURE_EXIT("Opening: %s failed\n",file_name); 
    
    
    mutexes = calloc(N,sizeof(pthread_mutex_t));
    for(int i=0;i<N;i++){
        pthread_mutex_init(mutexes+i,NULL);
    }

    pthread_cond_init(&not_empty,NULL);
    pthread_cond_init(&not_full,NULL);

    producer_threads = calloc(P,sizeof(pthread_t));
    consumer_threads = calloc(K,sizeof(pthread_t));

    for(int p=0;p<P;p++){
        pthread_create(&producer_threads[p],NULL,doProducerWork,NULL);
    }
    for(int k=0;k<K;k++){
        pthread_create(&consumer_threads[k],NULL,doConsumerWork,NULL);  
    }
    //if(atexit(releaseResources)!=0) FAILURE_EXIT("Failed to set atexit function\n");
    printf("2\n");



    for(int p=0;p<P;p++){
        if(pthread_join(producer_threads[p],NULL)!=0) FAILURE_EXIT("Waiting for producer thread failed: %s\n",strerror(errno));
    }
    finish_work=1;
    for(int k=0;k<K;k++){
        if(pthread_join(consumer_threads[k],NULL)!=0) FAILURE_EXIT("Waiting for consumer thread failed: %s\n",strerror(errno));
    }
    

    fclose(file);
}



