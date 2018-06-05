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


void printInfo(){
    printf("Please enter correct arguments:\n");
    printf("./zad <number of threads> <picture path> <filter path> <output path>\n");
    printf("./zad 5 ../dog.pgm ../edge_detection.txt ../newdog.pgm\n");
    exit(0);
}

void parseCommandArgs(int argc, char * argv[]){
    if(argc !=5) printInfo();
    number_of_threads = atoi(argv[1]);
    picture_path = argv[2];
    filter_path = argv[3];
    new_picture_path = argv[4];

}


int main(int argc, char * argv[]){
    parseCommandArgs(argc,argv);
    readFromPictureFile();
    readFromFilterFile();
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



