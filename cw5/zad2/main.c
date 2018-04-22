#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#define WRITE_MSG(format,...) {char buffer[255];sprintf(buffer,format, ##__VA_ARGS__);write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(code, format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(code);}
#define max_slaves 1000
int number_of_children;
int *children;
void printInfo(){
    printf("Example of usage:\n");
    printf("./zad <path where fifo will be crated> <number of slaves> <slave\'s loop>\n");
    exit(-1);
}

void SIGINTHandler(int signo, siginfo_t* info, void* context){
    for(int i=0;i<number_of_children;i++){
        kill(SIGSTOP,children[i]);
    }
    exit(-1);
}




int main(int argc,char *argv[]){
    number_of_children = atoi(argv[2]);
    unlink(argv[1]);
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER|SA_SIGINFO;
    act.sa_sigaction = SIGINTHandler;
    if(sigaction(SIGINT,&act,NULL) == -1)  printf("Cannot catch SIGINT");

    children = calloc(number_of_children,sizeof(int));
    int k=0;

    if(argc!=4) printInfo();

    int N = atoi(argv[2]);


    pid_t pid = fork();
    if(pid ==0){
        execlp("./master","master",argv[1],NULL);
        FAILURE_EXIT(-1,"Master failed\n");
    }
    children[k++]=pid;
    usleep(10000); //I want to be sure that master had enough time to created pipe before children opened it
    for(int i=0;i<N;i++){
        pid_t pid = fork();
        if(pid ==0){
            execlp("./slave","slave",argv[1],argv[3],NULL);
            FAILURE_EXIT(-1,"Slave failed\n");
        }
        children[k++] = pid;
    }

    while(wait(NULL)>0);

    return 0;
}

