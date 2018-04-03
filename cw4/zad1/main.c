#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>



void error(char *array){
    printf("ERROR: %s",array);
    exit(-1);
}

volatile int stopped=0;
pid_t pid;

void sigint(int signum){
    printf("Received signal SIGINT\n");
    if(!stopped) kill(pid,SIGSTOP);
    exit(0);

}

void sigtstop(int signum){

    if(stopped){
        if((pid = fork()) == 0){
            execl("../date.sh","../date.sh",NULL);
            exit(0);
        }
        stopped=0;
    }else{
        printf("Waiting for CTRL+Z continue or CTRL-C exit\n");
        kill(pid,SIGSTOP);
        stopped=1;
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa,NULL);

    signal(SIGTSTP,&sigtstop);

    if((pid = fork()) == 0){
        execl("../date.sh","../date.sh",NULL);
    }

    while(1){

    }

}