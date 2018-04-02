#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){

    char *foo = malloc(1000000000);
    for(int i=0;i<1000000000;i++){
        foo[i]='g';
    }

    return 0;
}