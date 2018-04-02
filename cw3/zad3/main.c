#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


#define max_length_of_line 100
#define max_number_of_words_in_line  100

typedef struct {
    struct timeval user;
    struct timeval system;
} Time;



Time measureTime(){
    Time T;

    struct rusage tmp;
    getrusage(RUSAGE_SELF, &tmp);

    T.user = tmp.ru_utime;
    T.system = tmp.ru_stime;
    return T;
}

void measure

void printTimeDifference(Time T2, Time T1){

    if(T2.user.tv_usec >=T1.user.tv_usec){
        printf("user: %ld.%06ld s\n", T2.user.tv_sec - T1.user.tv_sec, T2.user.tv_usec - T1.user.tv_usec);
    }else{
        printf("user: %ld.%06ld s\n", T2.user.tv_sec - T1.user.tv_sec-1, 1000000+T2.user.tv_usec - T1.user.tv_usec);
    }
    if(T2.system.tv_usec >=T1.system.tv_usec){
        printf("system: %ld.%06ld s\n", T2.system.tv_sec - T1.system.tv_sec, T2.system.tv_usec - T1.system.tv_usec);
    }else{
        printf("system: %ld.%06ld s\n", T2.system.tv_sec - T1.system.tv_sec-1, 1000000+T2.system.tv_usec - T1.system.tv_usec);
    }
}


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

        struct rlimit lim = {1, 1};
        if (setrlimit(RLIMIT_STACK, &lim) == -1) {

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

    return 0;
}