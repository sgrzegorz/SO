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
int P, K, N;
char file_name[40];
int search_mode,print_mode,nk;


void printInfo(){
    printf("Please enter correct arguments:\n");
    printf("./zad <configuration file path>\n");
    printf("./zad ../configuration.txt \n");
    exit(0);
}

void parseCommandArgs(int argc, char * argv[]){
    if(argc !=1) printInfo();
    
    FILE *file;
    if(file = fopen(argv[1],"r")){
        
        if(fscanf("%i %i %i %s %i %i %i",&P,&K,&N,file_name,&search_mode,&print_mode,&nk)==-1) FAILURE_EXIT("Error while parsing:%s",argv[1]);

        fclose(file);
    }

}

void * produce(void * thread_i){
    int thread_id = (intptr_t) thread_i;
}

void * consume(void *thread_i){
    int thread_id = (intptr_t) thread_i;

}


int main(int argc, char * argv[]){
    parseCommandArgs(argc,argv);
    
    int i;
    for(int i;i<P;i++){
        pthread_create(&threads[i],NULL,produce,(void *) (intptr_t) i);
    }
    for(;i<K+P;i++){
        pthread_create(&threads[i],NULL,consume,(void *) (intptr_t) i);
    }
    

    
   
    if(C > W || C >H) FAILURE_EXIT("Filter is bigger than picture \n");
     
    threads = calloc(number_of_threads,sizeof(pthread_t));
    Time T1 = measureTime();
    for(int i=0;i<number_of_threads;i++){
        pthread_create(&threads[i],NULL,filterPicture,(void *) (intptr_t) i);
    }


    for(int i=0;i<number_of_threads;i++){
        if(pthread_join(threads[i],NULL)!=0) FAILURE_EXIT("Waiting for thread failed\n");
    }
    Time T2 = measureTime();
    printTimeDifference(T2,T1);
    saveResults();
    printf("Picture ready in: %s \n",new_picture_path);
}



