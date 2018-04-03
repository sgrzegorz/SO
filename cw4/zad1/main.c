#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <zconf.h>



void error(char *array){
    printf("ERROR: %s",array);
    exit(-1);
}

volatile int stopped=0;

void handler(int signum){
    printf("Received signal SIGINT\n");
    exit(0);

}

void handler1(int signum){

    if(stopped){
        stopped=0;
        return;
    }else{
        printf("Waiting for CTRL+Z continue or CTRL-C exit\n");
        stopped=1;
    }
}

main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa,NULL);

    signal(SIGTSTP,&handler1);

    while(1){
        printf("In loop\n");
        sleep(1);
        if(stopped) pause();
    }

}