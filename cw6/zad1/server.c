#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "settings.h"
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
#define W(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
//sudo ipcrm --all=msg
int server_queue;
Message msg;
int client[MAXCLIENTS];
int active_clients = 0;

void intHandler(int dummy) {
    if(msgctl(server_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete server queue from handler: %s\n",strerror(errno) );
    exit(0);
}

void addNewClient(){
    int client_queue = msg.client_queue;
    W("%d d\n",client_queue);
    if(active_clients >= MAXCLIENTS) FAILURE_EXIT("Too many clients\n");
    for(int i=0;i<MAXCLIENTS;i++) {
        if (client[i] != -1) {
            client[i] = client_queue;
            break;
        }
    }
    msg.client_queue = client_queue;
    msgsnd(client_queue,&msg,MSG_SIZE,0);

}

void handleMirror(){
    int j=0;
    char buff[TEXT_SIZE];
    for(int i=strlen(msg.text);i>=0;i--){
        buff[j++] = msg.text[i];
    }

    strcpy(msg.text,buff);
    printf("%s",buff);
    msgsnd(msg.client_queue,&msg,MSG_SIZE,0);
}

void handleCalc(){


    switch(type){
        case strcmp(type,"ADD") == 0:
            break;
        case strcmp(type,"MUL") == 0:
            break;
        case strcmp(type,"SUB") == 0:
            break;
        case strcmp(type,"DIV") == 0:
            break;
        default:
            FAILURE_EXIT("Incorrect calculation type\n");
    }
}

void handleTime(){


}

void removeClient(){

}

int main() {
    signal(SIGINT, intHandler);

    key_t public_key = ftok(getenv("HOME"), PROJECT_ID);
    server_queue = msgget(public_key, IPC_CREAT | IPC_EXCL|0777);
    if (server_queue == -1) FAILURE_EXIT("server_queue wasn't created: %s\n", strerror(errno));

    for(int i=0;i<MAXCLIENTS;i++){
        client[i]=-1;
    }


    while(1){
        WRITE_MSG("Server waits for message\n");
        int result = msgrcv(server_queue,&msg,MSG_SIZE,0,0);
        if(result <0) FAILURE_EXIT("%s\n","Problem with main server loop");

        WRITE_MSG("Server reveived: %ld %s\n",msg.type,msg.text);

        W("1111\n");
        switch(msg.type){

            case HELLO:
            W("d\n");
                addNewClient();
                break;
            case MIRROR:
                handleMirror();
                break;
            case CALC:
            W("dd1\n");
                handleCalc();
                break;
            case TIME:
                handleTime();
                break;
            case END:
            W("dvv\n");
                removeClient();
                break;

        }

    }


}

