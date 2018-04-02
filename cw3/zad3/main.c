#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


#define max_length_of_line 100
#define max_number_of_words_in_line  100


void printTimeDifference(struct rusage T2, struct rusage T1,char *words[],int number_of_arguments){
    for(int i=0;i<number_of_arguments;i++){
        printf("%s ",words[i]);
    }

    if(T2.ru_utime.tv_usec >=T1.ru_utime.tv_usec){
        printf(" user: %ld.%06ld s, ", T2.ru_utime.tv_sec - T1.ru_utime.tv_sec, T2.ru_utime.tv_usec - T1.ru_utime.tv_usec);
    }else{
        printf(" user: %ld.%06ld s, ", T2.ru_utime.tv_sec - T1.ru_utime.tv_sec-1, 1000000+T2.ru_utime.tv_usec - T1.ru_utime.tv_usec);
    }
    if(T2.ru_stime.tv_usec >=T1.ru_stime.tv_usec){
        printf("system: %ld.%06ld s\n", T2.ru_stime.tv_sec - T1.ru_stime.tv_sec, T2.ru_stime.tv_usec - T1.ru_stime.tv_usec);
    }else{
        printf("system: %ld.%06ld s\n", T2.ru_stime.tv_sec - T1.ru_stime.tv_sec-1, 1000000+T2.ru_stime.tv_usec - T1.ru_stime.tv_usec);
    }
}

void setRestrictions(char *time, char *virtual_memory){
    struct rlimit lim;
    lim.rlim_cur = atol(time);
    lim.rlim_max =atol(time);
    setrlimit(RLIMIT_CPU,&lim);
    lim.rlim_cur = atol(virtual_memory);
    lim.rlim_max = atol(virtual_memory);
    setrlimit(RLIMIT_AS,&lim);

}

int main(int argc, char *argv[]){

    if(argc!=4){
        printf("Correct format: <file.txt> <time> <byte>\n");
        exit(-1);
    }

    FILE *file = fopen(argv[1],"r");
    if(file == NULL){
        printf("Opening of a file wasn't possible");
        exit(-1);
    }

    char line[max_length_of_line];
//    for(int i=0;i<max_length_of_line;i++){
//        line[i]='x';
//    }

    struct rusage T1, T2;

    char *words[max_number_of_words_in_line];
    while(fgets(line,max_length_of_line,file) != NULL){
        if(line[0] =='\n') continue;
        if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';


        int number_of_arguments=0;
        for(char *t = strtok(line," ");t!=NULL;t = strtok(NULL," ")){
            words[number_of_arguments++] = t;
        }
        words[number_of_arguments] = NULL; //it's needed by execvp


        getrusage (RUSAGE_CHILDREN, &T1);
        pid_t pid = fork();
        if(pid == 0){
            setRestrictions(argv[2],argv[3]);
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

        getrusage(RUSAGE_CHILDREN,&T2);
        printTimeDifference(T2,T1,words,number_of_arguments);

//        for(int i=0;i<max_length_of_line;i++){
//            line[i]='x';
//        }

    }
    fclose(file);
    return 0;
}