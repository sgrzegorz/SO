#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
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


    if(client_queue = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL|0666) == -1) FAILURE_EXIT("%s\n","client_queue wasn't created");

    Message  *msg = calloc(1,sizeof(Message));
    msg->type=0;

    strcpy(msg->text,"adffaffa");
    key_t public_key = ftok( getenv("HOME"),PROJECT_ID);
    server_queue = msgget(public_key, 0);

    msgsnd(server_queue,&msg,sizeof(msg->text),IPC_NOWAIT);

    printf("%d\n",client_queue);


    if(msgctl(client_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete client queue: %s\n",strerror(errno) );
    return 0;
}



