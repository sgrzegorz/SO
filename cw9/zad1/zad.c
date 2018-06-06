#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#define _GNU_SOURCE
#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}

int N;
char *buffor[4096];
int L;
int P, K, N;
char file_name[40];
int search_mode,print_mode,nk;
int  consument_index, producer_index;
mutex_t *mutex;
cond_t not_full, not_empty;
FILE *file;


typedef struct{
    char* array[MAX_BUFFER_SIZE];
    int consume_i ,produce_i, nelements, size;
    
}Buffer;

Buffer buf;

int produce(Buffer *buf,char * string){
    if(buf ->nelements == buf->size) return -1; //no place to push 
    buf->array[buf->produce_i] = val;
    buf -> produce_i = (buf->produce_i +1) % buf->size;
    
    buf ->nelements++;
    return 0;
}

int * consume(Buffer * buf){
    if(buf ->nelements == 0) return -1; // no element to pop
    char* string =  buf->array[consume_i];
    buf->array[consume_i] = NULL;
    buf->consume_i = (buf->consume_i +1) % buf->size;
    buf-> nelements--;

    switch(search_mode){
        case -1:
            if(strlen(string) < 
        case 0:

        case 1:

    }

    
        
    return val;
}


void * doProducerWork(void * thread_i){
    int thread_id = (intptr_t) thread_i;
    pthread_mutex_lock(&buffer[producer_index]);
    while(getSize(fifo) == QUEUE_SIZE){
        pthread_cond_wait(&not_full);
    }
    
    push(fifo);


    thread_cond_signal(&not_empty);
    pthread_mutex_unlock();
}

void * doConsumerWork(void *thread_i){
    int thread_id = (intptr_t) thread_i;
    pthread_mutex_lock(&buffer[consument_index]);
    while(getSize(fifo) == 0){
        pthread_cond_wait(&not_empty);
    }
    pop(fifo);


    pthread_cond_signal(&not_full);
    pthread_mutex_unlock();
}

void printInfo(){
    printf("Please enter correct arguments:\n");
    printf("./zad <buffer size> <L> <configuration file path>\n");
    printf("<L>: -1 lower , 0 equal, 1 greater\n");
    printf("Configuration args: P K N file_name search_mode print_mode nk\n")
    printf("./zad 45 -1 ../configuration.txt \n");
    exit(0);
}

void parseCommandArgs(int argc, char * argv[]){
    if(argc !=3) printInfo();
    arg
    FILE *file;
    if((file = fopen(argv[1],"r") == NULL) FAILURE_EXIT("Opening configuration file failed\n");
    int N; 
    if(fscanf("%i %i %i %s %i %i %i",&P,&K,&N,file_name,&search_mode,&print_mode,&nk)==-1) FAILURE_EXIT("Error while parsing:%s",argv[2]);
    
    if(N >= QUEUE_SIZE) FAILURE_EXIT("Given buffer size can't be larger than %i",MAX_BUFF_SIZE);
    buf->size = N;
    buf->produce_i = 0;
    buf->consume_i = 0;
    buf->nelements =0;
    for(int i=0;i<size;i++){
        array[i] = NULL;
    }

    fclose(file);
}


int main(int argc, char * argv[]){
    parseCommandArgs(argc,argv);
    if((file = fopen(file_name,"r")) == NULL) FAILURE_EXIT("Opening: %s failed\n",file_name); 
    pthread_mutex_init()

    mutex = calloc(N, )

    threads = calloc(number_of_threads,sizeof(pthread_t));
    int i;
    for(int i;i<P;i++){
        pthread_create(&threads[i],NULL,doProducerWork,(void *) (intptr_t) i);
    }
    for(;i<K+P;i++){
        pthread_create(&threads[i],NULL,doConsumerWork,(void *) (intptr_t) i);  
    }
     
    




    for(int i=0;i<number_of_threads;i++){
        if(pthread_join(threads[i],NULL)!=0) FAILURE_EXIT("Waiting for thread failed\n");
    }
    

    fclose(file);
}



