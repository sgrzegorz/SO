#include <stdio.h>
#include <stdlib.h>
#include "array.h"

#define  GLOBAL_SIZE 1000000
#define INFTY 2147483647

char global_array[GLOBAL_SIZE];


void error(char *array){
    printf("ERROR: %s",array);
    exit(-1);
}


Array initArray(int size, int block_size, int is_static){
    struct Array Array = {NULL,NULL,NULL,-1,-1,-1};
    Array.size = size;
    Array.block_size = block_size;
    Array.is_static = is_static;
    Array.is_used=malloc(sizeof(int)*Array.size);
    int i;
    for(i=0;i<Array.size;i++){
        Array.is_used[i] =0;
    }

    if(Array.is_static){ // static
        if(Array.size >GLOBAL_SIZE) error("invalid GLOBAL_SIZE");

        Array.stat=global_array;
        Array.dyn =NULL;

    }else{ //dynamic

        Array.dyn = malloc(Array.size * sizeof(char*));
        int i;
        for(i=0;i<Array.size;i++){
            Array.dyn[i] = NULL;
        }
        Array.stat =NULL;


    }
    return Array;
}

void delArray(Array Array){
    if(Array.is_static){  //static
        int i;
        for(i=0 ; i<Array.size; i++ ){
            Array.stat = '\0';
        }
        Array.stat = NULL;

    }else{ //dynamic
        int i;
        for(i = 0; i<Array.size;i++){
            free(Array.dyn[i]);
        }
        free(Array.dyn);
        Array.dyn = NULL;
    }
    Array.size =-1;
    Array.block_size=-1;
    Array.is_static=-1;
    free(Array.is_used);
    Array.is_used=NULL;
}

char* genRandom(Array Array) {
    static const char alphanum[] =     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char *s = malloc(Array.block_size*sizeof(char));
    for (int i = 0; i < Array.block_size; i++) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return s;
}

void addBlock(Array Array, int number){
    if(Array.size == -1) error("Array not initialized");
    if(!(number < Array.size)) error("addBlock index is bigger than Array.size ");
    if(Array.is_used[number]) error("addBlock index in array is already occupied ");


    if(Array.is_static){
        int isused =0; // check if global_array is big enough to add new block
        for(int i=0;i<Array.size;i++){
            if(Array.is_used[i]) isused++;
        }
        if((isused +1)*Array.block_size > GLOBAL_SIZE) error("you try to add more elements than global size can contain");

        char *t = genRandom(Array);
        for(int i=0;i<Array.block_size;i++){
            Array.stat[number*Array.block_size+i] = t[i];
        }
        free(t);
        t=NULL;

    }else{

        Array.dyn[number] = malloc(Array.block_size*sizeof(char));
        char* t = genRandom(Array);
        int j;
        for(j=0;j<Array.block_size;j++){
            Array.dyn[number][j] = t[j];
        }

        free(t);
        t =NULL;

    }
    Array.is_used[number] =1;

}


void delBlock(Array Array,int number){

    if(Array.size == -1) error("Array not initialized");
    if(number >= Array.size || number < 0 ) error("deleteBlock invalid argument");
    if(Array.is_used[number] == 0) error("deleteBlock trying to delete nonexisting block");

    if(Array.is_static){
        int i;
        for(i=0;i<Array.block_size;i++){
            Array.stat[number*Array.block_size+i] = '\0';
        }
    }else{
        free(Array.dyn[number]);
        Array.dyn[number] = NULL;
    }
    Array.is_used[number] =0;
}

int search(Array Array,int value){
  
   
     
    if(Array.size == -1) {
        error("Array not initialized");
        
    }
   
    //check arg
     
        int i;
        int used_block_number=0;
        printf("%d\n",used_block_number );
        for(i=0;i<Array.size;i++){
            if(Array.is_used[i]==1) used_block_number++;

        }
        
      
        if(used_block_number == 0) error("Search array is empty and you try to find block");
        printf("%d\n",used_block_number );
    
 
    int sum[Array.size];  //array containg sum of elements for each block
    
    for(i=0;i<Array.size;i++)
    {
        sum[i]=0;
    }

    if(Array.is_static){ //fill sum[] when array is static
        int i;
        for(i=0;i<Array.size;i++){

            if(Array.is_used[i]){
                int j;
                for(j=0;j<Array.block_size;j++){
                    sum[i]+=Array.stat[i+j];
                }
            }else{
                sum[i]=0;
            }
        }

    }else{ //fill sum[] when array is dynamic

        int i;
        for(i=0; i<Array.size;i++){
            if(Array.is_used[i]){

                int j;
                for(j=0;j<Array.block_size;j++){
                    sum[i]+=Array.dyn[i][j];
                }
            }else{
                sum[i]=0;
            }
        }
    }
    int nr=-1;       // indeks of block we're looking for
    int diff=INFTY; // difference between sum of characters in this block and a given value
    for(i=0;i<Array.size;i++){
        if(abs(sum[i]-value) <diff){
            diff = abs(sum[i]-value);
            nr = i;

        }
    }

    return nr;
}
void show(Array Array){
    printf("\n------------------------------\n");
    printf("size: %d\n",Array.size);
    printf("block_size: %d\n",Array.block_size);
    printf("is_static: %d\n",Array.is_static);
    printf("which blocks are used:\n");
    int used =0;
    int i;
    for(i=0;i<Array.size;i++){
        printf("%d ",Array.is_used[i]);
        if(Array.is_used[i]) used++;
    }
    printf("\nnumber of used blocks: %d\n-------------------------\n",used);
}

