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


    char str[BUFFSIZE];
    char *words[MAX_NUM_OF_WORDS_IN_LINE];
    while(fgets(str,BUFFSIZE,file) != NULL){

        if(str[strlen(str)-1] == '\n') str[strlen(str)-1] = '\0';  //strtok saves \n

        int number_of_arguments=0;
        for(char *t = strtok(str," ");t!=NULL;t = strtok(NULL," ")){
            words[number_of_arguments++] = t;
        }
        words[number_of_arguments] = NULL;

        //printf("%s",words[0]);

        char *argv[] = {"ls", "-l", 0};
        pid_t pid = vfork();
        if(pid == 0){
            execvp(words[0], words);
            exit(0);
        }
        int status;
        waitpid(-1,&status,0);
        if(status){
            error("You have put incorrect line");
            for(int i=0;i<number_of_arguments;i++){
                printf("%s ",words[i]);
            }
        }

    }
    //if(fgets(str,BUFFSIZE,file) == NULL) error("fgets error");



    fclose(file);



    return 0;
}