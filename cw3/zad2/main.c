#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#define max_length_of_line 100
#define max_number_of_words_in_line  100



int main(int argc, char *argv[])
{
    if(argc!=2){
        printf("Incorrect number of arguments\n");
        exit(-1);
    }

    FILE *file = fopen(argv[1],"r");
    if(file == NULL){
        printf("Opening of a file wasn't possible");
        exit(-1);
    }

    char str[max_length_of_line];
    char *words[max_number_of_words_in_line];
    while(fgets(str,max_length_of_line,file) != NULL){

        if(str[strlen(str)-1] == '\n') str[strlen(str)-1] = '\0';

        int number_of_arguments=0;
        for(char *t = strtok(str," ");t!=NULL;t = strtok(NULL," ")){
            words[number_of_arguments++] = t;
        }
        words[number_of_arguments] = NULL; //it's needed by execvp, moreover if number_of_arguments == 0,
                                            //execvp(NULL,NULL) is called. Because of that in file empty lines are allowed.

        pid_t pid = fork();
        if(pid == 0){
            execvp(words[0], words);
            exit(0);
        }

        if(wait(NULL)==-1){
            printf("You have put incorrect line: \n");
            for(int i=0;i<number_of_arguments;i++){
                printf("%s ",words[i]);
            }
            return -1;

        }

    }

    fclose(file);

    return 0;
}