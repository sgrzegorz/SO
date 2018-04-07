#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

volatile int live = 1;


void parentAsksChildToDieHandler(int signo){
    live = 0;
}

void parentAcceptedRequestHandler(int signo){
    printf("My parent noticed me: %d \n",getpid());
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
    live = 0;
}

int main() {
    time_t t;
    srand((unsigned int) getpid()+ time(NULL));

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = parentAcceptedRequestHandler;
    sigaction(SIGUSR1,&act,NULL);
    act.sa_handler = parentAsksChildToDieHandler;
    sigaction(SIGINT,&act,NULL);

    int length_of_sleeping = rand() % 4;
    printf("I live: %d, sleeps %ds \n",getpid(),length_of_sleeping);
    sleep(length_of_sleeping);
    kill(getppid(),SIGUSR1);

    while(live){

    }
    printf("COS%d\n",length_of_sleeping);

    return length_of_sleeping;
}