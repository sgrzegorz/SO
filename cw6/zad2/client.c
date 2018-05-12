#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <zconf.h>
#include "settings.h"
#include <mqueue.h>


#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define W(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}

volatile mqd_t server_queue,client_queue;
Message msg;

void intHandler() {
    exit(0);
}

void atexitFunction() {
    msg.type = STOP;
    mq_close(server_queue);
    mq_close(client_queue);
    mq_send(server_queue,(char*) &msg, sizeof(Message),0);
    WRITE_MSG("Client is being closed\n");
}


int main() {
    atexit(atexitFunction);
    signal(SIGINT, intHandler);

    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Message);
    char client_name[40];
    sprintf(client_name,"/%d",getpid());
    client_queue = mq_open(client_name, O_RDONLY|O_CREAT,0666,&attr);
    if (client_queue == -1) FAILURE_EXIT("%s\n", "client_queue wasn't created");


    server_queue = mq_open("/server",O_WRONLY);
    if (server_queue == -1) FAILURE_EXIT("server_queue wasn't opened by client.\n");

    msg.type = HELLO;
    msg.pid = getpid();
    mq_send(server_queue,(char*) &msg, sizeof(Message),0);
    mq_receive(client_queue, (char*) &msg, sizeof(Message),0);
    printf("Client connected to server: %s\n", msg.text);

    while (1) {

        printf("Enter your command: ");
        char cmd[100];
        if (fgets(cmd, 100, stdin) == NULL) FAILURE_EXIT("No input\n");
        cmd[strlen(cmd) - 1] = '\0'; //remove new line

        char *type, *remainder, *token;
        type = strtok_r(cmd, " ", &token);
        remainder = token;


        if (strcmp(type, "MIRROR") == 0) {
            msg.type = MIRROR;
            strcpy(msg.text, remainder);
            mq_send(server_queue,(char*) &msg, sizeof(Message),0);
            mq_receive(client_queue, (char*) &msg, sizeof(Message),0);
            WRITE_MSG("%s", msg.text);

        } else if (strcmp(type, "CALC") == 0) {
            msg.type = CALC;
            strcpy(msg.text, remainder);
            mq_send(server_queue,(char*) &msg, sizeof(Message),0);
            mq_receive(client_queue, (char*) &msg, sizeof(Message),0);
            WRITE_MSG("%s\n", msg.text);

        } else if (strcmp(type, "TIME") == 0) {
            if (strlen(remainder) != 0) {
                printf("Incorrect argument\n");
                continue;
            }
            msg.type = TIME;
            mq_send(server_queue,(char*) &msg, sizeof(Message),0);
            mq_receive(client_queue, (char*) &msg, sizeof(Message),0);
            WRITE_MSG("%s", msg.text);
        } else if (strcmp(type, "END") == 0) {
            if (strlen(remainder) != 0) {
                printf("Incorrect argument\n");
                continue;
            }
            msg.type = END;
            mq_send(server_queue,(char*) &msg, sizeof(Message),0);
            exit(0);
        } else {
            printf("Incorrect argument\n");
        }


    }


    return 0;
}



// int main() {
//     mqd_t queue = mq_open("/server",O_WRONLY);
//     char buf[40];
//     Message msg;
//     strcpy(msg.text,"olamakota");
//     int communicate = mq_send(queue,(char*) &msg, sizeof(Message),0);
//     sleep(10);
//     mq_close(queue);
//     printf("%s<--",buf);
//     return 0;
// }



