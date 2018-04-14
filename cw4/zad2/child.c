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
    usleep(100000);
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER;

    act.sa_handler = childHandler;
    sigaction(SIGUSR1,&act,NULL);

    srand((unsigned int) getpid()+ time(NULL));
    int length_of_sleeping = rand() % 10000000;

    WRITE_MSG("I live: %d, sleeps %ds \n",getpid(),length_of_sleeping);
    usleep(length_of_sleeping);
//    WRITE_MSG("wwwwwwwwwwwwwwwwwwwwwww\n");
    kill(getppid(),SIGUSR1);

    while(1){

    }
    
    WRITE_MSG("I have to die...: %d, I slept: %d us\n",getpid(),length_of_sleeping);
    return length_of_sleeping;
}