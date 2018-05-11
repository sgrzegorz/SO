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
#include <zconf.h>
#include <time.h>

int end_task =0;
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
#define W(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
//sudo ipcrm --all=msg
int server_queue;
Message msg;
int client[MAXCLIENTS];
int active_clients = 0;

void intHandler(int dummy);
void addNewClient();
void handleMirror();
void handleCalc();
void handleTime();
void handleEND();


int main() {
    atexit(intHandler);
    signal(SIGINT, intHandler);

    key_t public_key = ftok(getenv("HOME"), PROJECT_ID);
    server_queue = msgget(public_key, IPC_CREAT | IPC_EXCL|0777);
    if (server_queue == -1) FAILURE_EXIT("server_queue wasn't created: %s\n", strerror(errno));

    for(int i=0;i<MAXCLIENTS;i++){
        client[i]=-1;
    }


    while(1){
        if(end_task){
            struct msqid_ds buf;
            msgctl(public_key,IPC_STAT,&buf);
            if (buf.msg_qnum == 0) break;
        }
        WRITE_MSG("Server waits for message\n");
        int result = msgrcv(server_queue,&msg,MSG_SIZE,0,0);
        if(result <0) FAILURE_EXIT("%s\n","Problem with main server loop");

        switch(msg.type){

            case HELLO:

                WRITE_MSG("Server received: HELLO from !%s!\n",msg.client_queue);
                addNewClient();
                break;
            case MIRROR:
                WRITE_MSG("Server received: MIRROR:!%s!\n",msg.text);
                handleMirror();
                break;
            case CALC:
                WRITE_MSG("Server received CALC:!%s!\n",msg.text);
                handleCalc();
                break;
            case TIME:
                WRITE_MSG("Server received TIME\n");
                handleTime();
                break;
            case END:
                WRITE_MSG("Server received END\n");
                end_task =1;
                break;

        }

    }


}

/////////////////////////////////////////////////////////////////////////////

void intHandler(int dummy) {
    WRITE_MSG("Server is closed\n");
    if(msgctl(server_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete server queue from handler: %s\n",strerror(errno) );
    exit(0);
}

void addNewClient(){
    int client_queue = msg.client_queue;

    if(active_clients >= MAXCLIENTS){
        WRITE_MSG("Too many clients\n");
        return;
    }
    for(int i=0;i<MAXCLIENTS;i++) {
        if (client[i] != -1) {
            client[i] = client_queue;
            break;
        }
    }
    int result = msgget(client_queue,0);
    if(result == -1) WRITE_MSG("Couldn't open client's queue !%d!\n",client_queue);
    msg.client_queue = client_queue;
    msgsnd(client_queue,&msg,MSG_SIZE,0);

}

void handleMirror(){
    WRITE_MSG("Server received: MIRROR:!%s!\n",msg.text);
    int j=0;
    char buff[TEXT_SIZE];
    for(int i=strlen(msg.text);i>=0;i--){
        buff[j++] = msg.text[i];
    }

    strcpy(msg.text,buff);
    printf("!%s!\n",buff);
    msgsnd(msg.client_queue,&msg,MSG_SIZE,0);
}

void handleCalc(){
    char *token;
    token = strtok(msg.text," ");
    char *type;
    int first,second;
    int loop=0;
    while( token != NULL ) {

        if(loop==0) type = token;
        if(loop==1) first = atoi(token);
        if(loop==2) second = atoi(token);
        if(loop==3){
            strcpy(msg.text,"Incorrect calc data1\n");
            msgsnd(msg.client_queue,&msg,MSG_SIZE,0);
            return;
        }
        token = strtok(NULL," ");
        loop++;
    }

    int result;
    if(strcmp(type,"ADD")==0){
        result = first+second;
    }else if(strcmp(type,"SUB")==0){
        result = first - second;
    }else if(strcmp(type,"MUL")==0){
        result = first *second;
    }else if(strcmp(type,"DIV")==0){
        if(second == 0){
            strcpy(msg.text,"Incorrect calc data\n");
            msgsnd(msg.client_queue,&msg,MSG_SIZE,0);
            return;
        }
        result = first / second;
    }else{
        strcpy(msg.text,"Incorrect calc data3\n");
        msgsnd(msg.client_queue,&msg,MSG_SIZE,0);
        return;
    }
    sprintf(msg.text, "%d", result);
    msgsnd(msg.client_queue,&msg,MSG_SIZE,0);

}

void handleTime(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strcpy(msg.text,("%d-%d-%d %d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min));
    msgsnd(msg.client_queue,&msg,MSG_SIZE,0);
}

void handleEND(){
    while(1){

    }
    exit(0);
}