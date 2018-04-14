#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <string.h>
volatile int live = 1;


void ps(char *s){
    char buff1[40];
    sprintf(buff1,s);
    strcat(buff1,"\n");
    write(1,buff1,strlen(buff1));
}



void childExitHandler(int signo){
    live = 0;
}

void parentAcceptedRequestHandler(int signo){

    //const char *str = "My parent noticed me\n";
   // write(1, str, strlen(str));
    //printf("My parent noticed me: %d \n",getpid());
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
    live = 0;
}



int main() {
    ps("I live");
    char buffer[100];
    time_t t;
    srand((unsigned int) getpid()+ time(NULL));

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    act.sa_handler = parentAcceptedRequestHandler;
    sigaction(SIGUSR1,&act,NULL);
    act.sa_handler = childExitHandler;
    sigaction(SIGINT,&act,NULL);

    int length_of_sleeping = rand() % 10000000;

    sprintf(buffer,"I live: %d, sleeps %ds \n",getpid(),length_of_sleeping);
    write(1, buffer, strlen(buffer));
    usleep(length_of_sleeping);
    kill(getppid(),SIGUSR1);

    while(live){

    }
    sprintf(buffer,"I have to die...: %d, I slept: %d us\n",getpid(),length_of_sleeping);
    write(1, buffer, strlen(buffer));
    return length_of_sleeping;
}