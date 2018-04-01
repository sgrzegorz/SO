#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


#ifndef DYNAMIC

#include "array.h"
#endif



#ifdef DYNAMIC
typedef struct Array {
   char *stat ;
   char **dyn ;
   int *is_used;
   int size;
   int block_size;
   int is_static;
}Array;


#include <dlfcn.h>
Array (*initArray)(int, int, int );
void (*delArray)(Array);
void (*addBlock)(Array,int);
void (*delBlock)(Array,int);
int (*search)(Array,int);
void (*show)(Array);
void (*error)(char *);


#endif


struct timeval real1;
struct timeval real2;
struct rusage com1;
struct rusage com2;

void time1(){
   
    gettimeofday(&real1, NULL);
   
    getrusage(RUSAGE_SELF, &com1);

}

void time2(){
     gettimeofday(&real2, NULL);
     getrusage(RUSAGE_SELF, &com2);

    if(real2.tv_usec >=real1.tv_usec){
        printf("real: %ld.%06ld s\n", real2.tv_sec - real1.tv_sec, real2.tv_usec - real1.tv_usec);
    }else{
        printf("real: %ld.%06ld s\n", real2.tv_sec - real1.tv_sec -1, (1000000+real2.tv_usec) - real1.tv_usec);
    }
    if(com2.ru_utime.tv_usec >=com1.ru_utime.tv_usec){
        printf("user: %ld.%06ld s\n", com2.ru_utime.tv_sec - com1.ru_utime.tv_sec, com2.ru_utime.tv_usec - com1.ru_utime.tv_usec);
    }else{
        printf("user: %ld.%06ld s\n", com2.ru_utime.tv_sec - com1.ru_utime.tv_sec-1, 1000000+com2.ru_utime.tv_usec - com1.ru_utime.tv_usec);
    }
    if(com2.ru_stime.tv_usec >=com1.ru_stime.tv_usec){
        printf("system: %ld.%06ld s\n", com2.ru_stime.tv_sec - com1.ru_stime.tv_sec, com2.ru_stime.tv_usec - com1.ru_stime.tv_usec);
    }else{
        printf("system: %ld.%06ld s\n", com2.ru_stime.tv_sec - com1.ru_stime.tv_sec-1, 1000000+com2.ru_stime.tv_usec - com1.ru_stime.tv_usec);
    }
}


Array timeCreateTable(int size,int block_size,int is_static){
    printf("Create table:\n");
    time1();
    Array Array =(*initArray)( size,block_size,is_static);
    time2();
    return Array;
}

void timeSearchElement(Array Array,int value){
    printf("\nSearch element:\n");
    time1();

    printf("Index: %d\n", search(Array,value));
    time2();
}

void timeRemoveNumber(Array Array,int number){

    printf("\nRemove number:\n");

    time1();

    int i;  // delete {number} of blocks

    for(i=0;i<number;i++){
        int j;
        int fdeleted = 0;

        for(j=0;j<Array.size;j++){
            if(Array.is_used[j]){
                delBlock(Array,j);
                fdeleted =1;
                break;
            }
        }

        if(!fdeleted) error("trying to remove more elements than array contains");

    }
    time2();
}

void timeAddNumber(Array Array,int number){
    
    
    printf("\nAddNumber:\n");
    time1();
    
    int i; // add {number} of blocks to an array
    for(i=0;i<number;i++){
        int fadded =0;
        int j;
        for(j=0;j<Array.size;j++){
            if(!Array.is_used[j]){
                addBlock(Array,j);
                fadded =1;
                break;
            }

        }

        if(!fadded) error("trying do add more blocks than an array can contain");
    }
    time2();
}

void timeAddAndRemove(Array Array,int number){


    printf("\nAdd {number} of blocks, remove {number} of {block}:\n");
    time1();

    int i; // add {number} of blocks to an array
    for(i=0;i<number;i++){
        int fadded =0;
        int j;
        for(j=0;j<Array.size;j++){
            if(!Array.is_used[j]){
                addBlock(Array,j);
                fadded =1;
                break;
            }
        }
        if(!fadded) error("trying do add more blocks than an array can contain");
    }

    for(i=0;i<number;i++){    // delete {number} of blocks added above
        int j;
        for(j=0;j<Array.size;j++){
            if(Array.is_used[j]){
                delBlock(Array,j);
                break;
            }

        }
    }

    time2();


    printf("\nAdd block, remove block {number} of times:\n");


    time1();
    int flag =0;
    for(i=0;i<Array.size;i++){
        if(!Array.is_used[i]){
            int j;
            for(j=0;j<number;j++){
                addBlock(Array,i);
                delBlock(Array,i);
            }
            flag =1;
            break;
        }
    }
    if(!flag) error("Array is full, adding and then removing block was impossible ");
    time2();
}



void parse(int argc,char*argv[]){
    if(argc == 1){
        printf("c {array_size} {block size} {is_static} - CREATE TABLE example: c 100 20 1  \n");
        printf("a {number}  - ADD NUMBER OF BLOCKS example: a 40\n");
        printf("r {number}  - REMOVE NUMBER OF BLOCKS example: r 40  \n");
        printf("ar {number}  - DO OPERATION OF REMOVING ADDING BLOCKS NUMBER TIMES example: ar 40  \n");
        printf("> {filename.txt}  save screen to file\n");
    }

    Array Array={NULL,NULL,NULL,-1,-1,-1};
    int i=1; //without path
    while(i<argc){
        if(strcmp(argv[i],"c") ==0){
            if(!(i+1 < argc || i+2 < argc ||i+3<argc)) error("invalid args");

            int size = atoi(argv[i+1]);
            int block_size = atoi(argv[i+2]);
            int is_static = atoi(argv[i+3]);
            Array = timeCreateTable(size,block_size,is_static);
            i+=4;

            continue;

        }else if(strcmp(argv[i],"s")==0){
            if(!(i+1 < argc)) error("invalid args");

            int sum = atoi(argv[i+1]);
            timeSearchElement(Array,sum);
            i+=2;


            continue;

        } else if(strcmp(argv[i],"r")==0){
            if(!(i+1 < argc)) error("invalid args");

            int number = atoi(argv[i+1]);

            timeRemoveNumber(Array,number);
            i+=2;

            continue;

        } else if(strcmp(argv[i],"a")==0){
            if(!(i+1 < argc)) error("invalid args");

            int number = atoi(argv[i+1]);

            timeAddNumber(Array,number);
            i+=2;

            continue;

        } else if(strcmp(argv[i],"ar")==0){

            if(!(i+1 < argc)) error("invalid args");
            int number = atoi(argv[i+1]);
            timeAddAndRemove(Array,number);
            i+=2;

            continue;

        } else{
            error("invalid args");
        }
    }

}



int main(int argc, char *argv[] )

{
    #ifdef DYNAMIC
    void *library;
    library = dlopen("./build/libshared.so", RTLD_NOW);
    if (!library) {
        printf("Failed to load: %s\n", dlerror());
        exit(-1);
    }

    initArray = (Array (*)(int, int,int)) dlsym(library, "initArray");
    delArray = (void (*) (Array)) dlsym(library, "delArray");
    addBlock = (void (*) (Array,int)) dlsym(library, "addBlock");
    delBlock = (void (*) (Array,int)) dlsym(library, "delBlock");
    search = (int (*) (Array,int) ) dlsym(library, "search");
    show = (void (*) (Array)) dlsym(library, "show");
    error = (void (*) (char *))  dlsym(library, "error");
    #endif
   
 
    parse(argc,argv);

    #ifdef DYNAMIC
    dlclose(library);
    #endif
    
    return 0;
}

