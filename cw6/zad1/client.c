#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <zconf.h>
#include "settings.h"
#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define W(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}

volatile int server_queue;
volatile int client_queue;
Message  msg;

void intHandler(int dummy) {
    if(msgctl(client_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete client queue from handler: %s\n",strerror(errno) );
    exit(0);
}

int main() {
    signal(SIGINT, intHandler);

    client_queue = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL|0666);
    if(client_queue == -1) FAILURE_EXIT("%s\n","client_queue wasn't created");




    key_t public_key = ftok( getenv("HOME"),PROJECT_ID);
    server_queue = msgget(public_key, 0);

    if (server_queue == -1) FAILURE_EXIT("server_queue wasn't opened by client.\n");



    msg.type= HELLO;
    msg.client_queue = client_queue;
    strcpy(msg.text,"");
    msgsnd(server_queue,&msg,MSG_SIZE,0);
    printf("fa\n");

    msgrcv(client_queue,&msg,MSG_SIZE,0,0);
    printf("%s<---",msg.text);

    msg.type=MIRROR;
    strcpy(msg.text,"adffaffa");
    msgsnd(server_queue,&msg,MSG_SIZE,0); //default block until is place in queue

    printf("%s\n",msg.text);


    sleep(3);
    if(msgctl(client_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete client queue: %s\n",strerror(errno) );
    return 0;
}



