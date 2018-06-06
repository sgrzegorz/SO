#include "shared_memory.h"


int produce(Buffer *buf,int val){
    if(buf ->nelements == buf->size) return -1; //no place to push 
    buf->array[buf->produce_i] = val;
    buf -> produce_i = (buf->produce_i +1) % buf->size;
    
    buf ->nelements++;
    return 0;
}

int * consume(Buffer * buf){
    if(buf ->nelements == 0) return -1; // no element to pop
    char* val =  buf->array[consume_i];
    buf->array[consume_i] = NULL;
    buf->consume_i = (buf->consume_i +1) % buf->size;
    buf-> nelements--;

    char * val = buf->queue[buf->head];
    buf->queue[buf->head] = -1;
    
    return val;
}

void init(Buffer * buf,int size){
    if(size >= QUEUE_SIZE) FAILURE_EXIT("Given buffer size can't be larger than %i",MAX_BUFF_SIZE);
	buf->size =size;
    buf->produce_i = 0;
    buf->consume_i = 0;
    buf->nelements =0;
    for(int i=0;i<size;i++){
        array[i] = NULL;
    }
   
}

