#include <stdio.h>
#include <stdlib.h>
#define BUFFSIZE 100
#define MAX_NUM_OF_WORDS_IN_LINE  100
#include <errno.h>
#include <unistd.h>
#include <string.h>


void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}




int main()
{


    FILE *file = fopen("../insert.txt","r");
    if(file == NULL) error("Opening of a file wasn't possible");



    char *str[BUFFSIZE];
    char *words[MAX_NUM_OF_WORDS_IN_LINE];
    if(fgets(str,BUFFSIZE,file) == NULL) error("fgets error");


    int i=0;
    char *t = strtok(str," ");
    while(t!=NULL){
        words[i++] = t;
        t = strtok(NULL," ");
    }
    words[i] = NULL;


    char *argv[] = {"ls", "-l", 0};

    execvp(words[0], words);


    fclose(file);



    return 0;
}