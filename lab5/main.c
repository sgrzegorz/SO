#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#define max_length_of_line 100
#define max_number_of_words_in_line  100

#define WRITE_MSG(format,...) {char buffer[255];sprintf(buffer,format, ##__VA_ARGS__);write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(code, format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(code);}

int getNumberOfCommands(const char string[],int length){
    if(string[0] == '|' || string[length-1] == '|') FAILURE_EXIT(-1,"error1 while parsing\n");

    int twoNullPointers = 0;
    int number_of_commands =0;
    for(int i=0;i<length;i++){
        if(string[i]=='|' && twoNullPointers) FAILURE_EXIT(-1,"error2 while parsing:\n");
        if(string[i]=='|'){
            twoNullPointers = 1;
            number_of_commands++;
        }
        if(string[i]!='|') twoNullPointers = 0;

    }
    number_of_commands+=1; //number of commands is one bigger than number of sign '|'
    return number_of_commands;
}

char** putPointersToSeparatedWords(char*string,int length,char **pointers,int number_of_commands){
    //char **pointers = calloc(number_of_commands*2,sizeof(char*));
    int p_i=0;

    int i=0;
    for(;i<length;i++){
        if(string[i] == ' ' || string[i]=='|'){
            string[i]='\0';
        }else{
            break;
        }
    }
    int ifstart = 1;
    for(;i<length;i++) {

        if(string[i] == '|') pointers[p_i++] = NULL;
        if(string[i] == ' ' || string[i]=='|'){
            string[i]='\0';
            ifstart = 1;
        }else if(string[i] != ' ' && string[i]!='|' && ifstart ){
            pointers[p_i++] = string+i;
            ifstart = 0;
        }else if(string[i] != ' ' && string[i]!='|' && !ifstart){
            continue;
        }
    }
    pointers[p_i++] = NULL;
    pointers[p_i++] = NULL;
    return pointers;
}


int main(int argc, char *argv[])
{
    //"ls   -l  /home/x/Desktop\0";
    char line[max_number_of_words_in_line]= "ps -aux | grep -a ala";
    int number_of_commands = getNumberOfCommands(line,strlen(line));

    char **word_pointers = calloc(sizeof(char*),max_number_of_words_in_line);
    word_pointers = putPointersToSeparatedWords(line,strlen(line),word_pointers,max_number_of_words_in_line);
    int word_pointers_length =0; //big enough
    for(int i=0;i<max_number_of_words_in_line;i++){
        if(word_pointers[i]==NULL && word_pointers[i+1]==NULL){
            word_pointers_length++;
            break;
        }
        word_pointers_length++;
    }


    for(int i=0;i<word_pointers_length;i++){
        if(word_pointers[i]!=NULL){
            printf("%s\n",word_pointers[i]);
        }
        else if(word_pointers[i]==NULL){
            printf("0\n");
        }else{
            printf("-\n");
        }
    }

    int command[number_of_commands];
    if(word_pointers[0]==NULL) FAILURE_EXIT(-1,"Error while parsing3\n");
    command[0] = 0;
    int j = 1;
    for(int i=1;i<word_pointers_length;i++){
        if(word_pointers[i-1] == NULL){
            command[j++] = i;
        }
    }
//    char ***command_pointers = calloc(number_of_commands,sizeof(char**));
//    command_pointers[0] = word_pointers;


    int fd[number_of_commands][2];
    for(int i=0;i<number_of_commands;i++){
        pipe(fd[i]);
    }
    dup2(fd[number_of_commands-1][1],STDOUT_FILENO);

    for(int i=0;i<number_of_commands;i++){
        pid_t pid = fork();
        if(pid == 0){
            if(i==0) {
                dup2(fd[i][1], STDOUT_FILENO);
            }else if(i == number_of_commands-1){
                dup2(fd[i][0], STDIN_FILENO);
            }else{
                dup2(fd[i][1],STDOUT_FILENO);
                dup2(fd[i][0],STDIN_FILENO);
            }
            read(0,200,200);
            execvp(word_pointers[command[i]],word_pointers+command[i]);
            FAILURE_EXIT(-1,"Problem with fork\n");
        }
        usleep(1000);
    }

    pid_t wpid;
    while(wpid = wait(NULL) >0);
    sleep(1);
    exit(1);
   /*
    char *pointersbuff=separateWordsInAString(t);
    char **pointers[3];
    for(int i=0;i<6)
    for(int i=0;i<3;i++){
        pid_t pid = fork();
        if(pid ==0){
            execlp(p,p)
            exit(0);
        }
    }
*/
/*
    int fd[3][2]
    for(int i=0;i<3;i++){
        pipe(fd[i]);
        pid_t pid = fork();
        if(pid == 0){
            dup2(fd[i][0],STDIN_FILENO);
            dup2(fd[i][1],STDOUT_FILENO);
            char t[50];
            read(fd[i][0],t,50);
            execlp(t,t,t,NULL);

            exit(0);
        }else{
            close(fd[0]);
            char *t[50];
            strcpy(t, "echo ewa | echo ewa| echo ewa");
            write(fd[1],t,strlen(t));
            dup2(fd[1],STDOUT_FILENO);
            WRITE_MSG("Parent dies\n");

        }
    }



/*
    if(argc!=2){
        printf("Incorrect number of arguments\n");
        exit(-1);
    }

    FILE *file = fopen(argv[1],"r");
    if(file == NULL){
        printf("Opening of a file wasn't possible");
        exit(-1);
    }

    char line[max_length_of_line];
    for(int i=0;i<max_length_of_line;i++){
        line[i]='x';
    }
    char *words[max_number_of_words_in_line];
    while(fgets(line,max_length_of_line,file) != NULL){
        if(line[0] =='\n') continue;
        if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';

        int number_of_arguments=0;
        for(char *t = strtok(line," ");t!=NULL;t = strtok(NULL," ")){
            words[number_of_arguments++] = t;
        }
        words[number_of_arguments] = NULL; //it's needed by execvp


        pid_t pid = fork();
        if(pid == 0){
            if(execvp(words[0], words) == -1){
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
        int status;
        if(waitpid(-1,&status,0)==-1){
            printf("error occured\n");
            exit(-1);
        }

        if(WIFEXITED(status) && WEXITSTATUS(status) !=0 ){
            printf("You have put incorrect line: \n");

            for(int i=0;i<number_of_arguments;i++){
                printf("%s ",words[i]);
            }
            printf("\n");
            exit(-1);

        }

        for(int i=0;i<max_length_of_line;i++){
            line[i]='x';
        }

    }

    fclose(file);
*/

    return 0;
}