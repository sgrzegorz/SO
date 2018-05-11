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
    WRITE_MSG("Client is closed\n");
    if(msgctl(client_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete client queue from handler: %s\n",strerror(errno) );
    exit(0);
}

void parseString(){

}

int main() {
    atexit(intHandler);
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
    msgrcv(client_queue,&msg,MSG_SIZE,0,0);


    while(1){

        printf("Enter your command: ");
        char cmd[100];
        if (fgets(cmd, 100, stdin) == NULL) FAILURE_EXIT("No input\n");
        cmd[strlen(cmd) - 1] = '\0'; //remove new line

        char *type, *remainder,*token;
        type = strtok_r (cmd, " ", &token);
        remainder = token;

        printf("!%s@%s!\n",cmd,remainder);
        if(strcmp(type,"MIRROR")==0){
            msg.type=MIRROR;
            strcpy(msg.text,remainder);
            msgsnd(server_queue,&msg,MSG_SIZE,0);
            msgrcv(client_queue,&msg,MSG_SIZE,0,0);
            WRITE_MSG("%s",msg.text);

        }else if(strcmp(type,"CALC")==0){
            msg.type=CALC;
            strcpy(msg.text,remainder);
            msgsnd(server_queue,&msg,MSG_SIZE,0);
            msgrcv(client_queue,&msg,MSG_SIZE,0,0);
            WRITE_MSG("%s",msg.text);

        }else if(strcmp(type,"TIME")==0){
            if(strlen(remainder)!=0){
                printf("Incorrect argument\n");
                continue;
            }
            msg.type=TIME;
            msgsnd(server_queue,&msg,MSG_SIZE,0);
            msgrcv(client_queue,&msg,MSG_SIZE,0,0);
            WRITE_MSG("%s",msg.text);
        }else if(strcmp(type,"END")==0) {
            if(strlen(remainder)!=0){
                printf("Incorrect argument\n");
                continue;
            }
            msg.type=END;
            msgsnd(server_queue,&msg,MSG_SIZE,0);
            exit(0);
        }else{
            printf("Incorrect argument\n");
        }


    }




    return 0;
}



