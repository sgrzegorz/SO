#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

typedef struct Array {
   char *stat ;
   char **dyn ;
   int *is_used;
   int size;
   int block_size;
   int is_static;
}Array;

Array initArray(int size, int block_size, int is_static);
void delArray(Array Array);
void addBlock(Array Array, int nr);
void delBlock(Array Array,int nr);
int search(Array Array, int sum);
void show(Array Array);
void error(char *array);


#endif // ARRAY_H_INCLUDED
