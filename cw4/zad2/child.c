#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}


void childHandler(int signo){
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
}



int main() {
    sleep(3);
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);

    act.sa_handler = childHandler;
    sigaction(SIGUSR1,&act,NULL);

    srand((unsigned int) getpid()+ time(NULL));
   // int length_of_sleeping = rand() % 1000000;
     int length_of_sleeping = rand() % 1000;
    WRITE_MSG("I live: %d, sleeps %ds \n",getpid(),length_of_sleeping);
    usleep(length_of_sleeping);
//    WRITE_MSG("wwwwwwwwwwwwwwwwwwwwwww\n");
    kill(getppid(),SIGUSR1);

    sigsuspend(&mask);
    return length_of_sleeping;
}