#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <zconf.h>
#include "settings.h"
#define FAILURE_EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
int server_queue;
int client_queue;

void intHandler(int dummy) {
    if(msgctl(client_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete client queue from handler: %s\n",strerror(errno) );
    exit(0);
}

int main() {
    signal(SIGINT, intHandler);

    client_queue = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL|0666);
    if(client_queue == -1) FAILURE_EXIT("%s\n","client_queue wasn't created");

    Message  msg;
    msg.type=1;

    strcpy(msg.text,"adffaffa");
    printf("%s",msg.text);
    key_t public_key = ftok( getenv("HOME"),PROJECT_ID);
    server_queue = msgget(public_key, 0);

    msgsnd(server_queue,&msg,sizeof(msg.text),0); //default block until is place in queue

    printf("%s\n",msg.text);

    sleep(3);
    if(msgctl(client_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete client queue: %s\n",strerror(errno) );
    return 0;
}



