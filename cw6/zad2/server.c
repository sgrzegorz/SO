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
#include <mqueue.h>


int end_task = 0;
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
#define W(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
//sudo ipcrm --all=msg
mqd_t server_queue;
Message msg;
int client[MAXCLIENTS][2];
int active_clients = 0;

void addNewClient();

void handleMirror();

void handleCalc();

void handleTime();

void handleEND();

int getQueueID();

void removeClient();

void atexitFunction() {
    for(int i=0;i<MAXCLIENTS;i++){
        if(client[i][0]!=-1){
            mq_close(client[i][1]);
            kill(client[i][0],SIGINT);
        }
    }

    WRITE_MSG("Server is being closed\n");
    mq_close(server_queue);
    if (mq_unlink("/server") == -1) FAILURE_EXIT("Couldn't delete server queue from handler: %s\n",
                                                                 strerror(errno));
}

void intHandler() {
    exit(0);
}


int main() {
    if (atexit(atexitFunction) == -1) FAILURE_EXIT("Registering client's atexit failed!\n");
    signal(SIGINT, intHandler);

    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Message);
    mqd_t server_queue = mq_open("/server", O_RDONLY|O_CREAT|O_EXCL,0666,&attr);
    if (server_queue == -1) FAILURE_EXIT("server_queue wasn't created: %s\n", strerror(errno));
    

    for (int i = 0; i < MAXCLIENTS; i++) {
        client[i][0] = -1;
        client[i][1] = -1;
    }


    while (1) {

        if (end_task) {
            struct mq_attr attr;
            mq_getattr(server_queue, &attr);
            if(attr.mq_curmsgs == 0) break;

        }
        WRITE_MSG("Server waits for message:\n");

        int result = mq_receive(server_queue, (char*) &msg, sizeof(Message),0);
        if (result < 0) FAILURE_EXIT("%s\n", "Problem with main server loop");

        switch (msg.type) {

            case HELLO: WRITE_MSG("Server received: HELLO\n");
                addNewClient();
                break;
            case MIRROR: WRITE_MSG("Server received: MIRROR\n");
                handleMirror();
                break;
            case CALC: WRITE_MSG("Server received: CALC\n");
                handleCalc();
                break;
            case TIME: WRITE_MSG("Server received: TIME\n");
                handleTime();
                break;
            case END: WRITE_MSG("Server received: END\n");
                end_task = 1;
                break;
            case STOP: WRITE_MSG("Server received: STOP\n");
                removeClient();

        }

    }


}

/////////////////////////////////////////////////////////////////////////////


void removeClient() {
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (client[i][0] == msg.pid) {
            mq_close(client[i][1]);
            client[i][0] = -1;
            client[i][1] = -1;
        }
    }
}


void addNewClient() {
    mqd_t client_queue = -1;
    if (active_clients >= MAXCLIENTS) {
        WRITE_MSG("Too many clients\n");
        kill(msg.pid, SIGINT);
        return;
    }

    for (int i = 0; i < MAXCLIENTS; i++) {
        if (client[i][0] == -1) {
            char buf[50];
            sprintf(buf,"/%d",msg.pid);
            client_queue = mq_open(buf,O_WRONLY);
            client[i][0] = msg.pid;
            client[i][1] = client_queue;

            if (client_queue == -1) {
                WRITE_MSG("Couldn't open client's queue !%d!\n", client_queue);
                kill(msg.pid, SIGINT);
                return;
            }

            break;
        }
    }


    char buf[40];
    sprintf(buf, "%d", msg.pid);
    strcpy(msg.text, buf);
    mq_send(client_queue,(char*) &msg, sizeof(Message),0);

}

void handleMirror() {

    int client_queue = getQueueID();
    char buff[TEXT_SIZE];
    int j = 0;
    for (int i = strlen(msg.text) - 1; i >= 0; i--) {
        buff[j++] = msg.text[i];
    }
    buff[j]=0;

    strcpy(msg.text, buff);
    mq_send(client_queue,(char*) &msg, sizeof(Message),0);
}

void handleCalc() {
    int client_queue = getQueueID();
    char *token;
    token = strtok(msg.text, " ");
    char *type;
    int first, second;
    int loop = 0;
    while (token != NULL) {

        if (loop == 0) type = token;
        if (loop == 1) first = atoi(token);
        if (loop == 2) second = atoi(token);
        token = strtok(NULL, " ");
        loop++;
    }
    printf("%d\n", loop);
    if (loop != 3) {
        kill(msg.pid, SIGINT);
        return;
    }

    int result;
    if (strcmp(type, "ADD") == 0) {
        result = first + second;
    } else if (strcmp(type, "SUB") == 0) {
        result = first - second;
    } else if (strcmp(type, "MUL") == 0) {
        result = first * second;
    } else if (strcmp(type, "DIV") == 0) {
        if (second == 0) {
            kill(msg.pid, SIGINT);
            return;
        }
        result = first / second;
    } else {
        kill(msg.pid, SIGINT);
        return;
    }
    sprintf(msg.text, "%d", result);
    mq_send(client_queue,(char*) &msg, sizeof(Message),0);

}

void handleTime() {
    int client_queue = getQueueID();
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char buf[TEXT_SIZE];
    sprintf(buf, "%d-%d-%d %d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
    strcpy(msg.text, buf);
    mq_send(client_queue,(char*) &msg, sizeof(Message),0);
}

int getQueueID() {
    int client_queue = -1;
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (client[i][0] == msg.pid) {
            client_queue = client[i][1];
            break;
        }
    }
    if (client_queue == -1) printf("Getting client_queue failed\n");
    return client_queue;
}



// int main() {
//     struct mq_attr attr;
//     attr.mq_maxmsg = 10;
//     attr.mq_msgsize = sizeof(Message);

//     mqd_t queue = mq_open("/server", O_RDONLY|O_CREAT,0666,&attr);
//     printf("%s\n",strerror(errno));
//     char buf[40];
//     int result = mq_receive(queue, (char*) &msg, sizeof(Message),0);
//     printf("%s\n",strerror(errno));
//     printf("!%s!,\n",msg.text);
//     //mq_close(queue);
//     mq_unlink("/server");

// }

/////////////////////////////////////////////////////////////////////////////

