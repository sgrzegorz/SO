#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

volatile int live = 1;


void parentAsksChildToDieHandler(int signo){
    live = 0;
}

void parentAcceptedRequestHandler(int signo){
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
    live = 0;
}

int main() {
    printf("I live\n");

    struct sigaction act;
    act.sa_handler = parentAcceptedRequestHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1,&act,NULL);

    act.sa_handler = parentAsksChildToDieHandler;
    sigaction(SIGINT,&act,NULL);

    int length_of_sleeping = rand() % 10;
    sleep(length_of_sleeping);
    kill(getppid(),SIGUSR1);

    while(live){

    }

    return length_of_sleeping;
}