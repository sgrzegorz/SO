#include <stdio.h>
#include <stdlib.h>

void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}


int main() {
    FILE *file = fopen("insert.txt","r");
    if(file == NULL) error "Couldn"



        fclose(file);

    printf("Hello, World!\n");
    return 0;
}