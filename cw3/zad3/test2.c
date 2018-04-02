#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
    int i= 1;
    while(1){
        long long int size =1024*1024;
        char *foo = malloc(size);
        for(long long int i=0;i<size;i++){
            foo[i]='g';
        }
        printf("%i, allocated 1MB;\n",i);
        i++;
    }

    return 0;
}