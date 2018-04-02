#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
    while(1){
        char *foo = malloc(10000000000000000000000); //10gb
        for(long long int i=0;i<     10000000000000000000000;i++){
            foo[i]='g';
        }


    }

    return 0;
}