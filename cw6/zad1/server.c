#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "settings.h"
#include <errno.h>
#include <memory.h>
#include <signal.h>

#define FAILURE_EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
//sudo ipcrm --all=msg
int server_queue;
Message msg;

void intHandler(int dummy) {
    printf("her\n");
    if(msgctl(server_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete server queue from handler: %s\n",strerror(errno) );
    exit(0);
}

int main() {
    signal(SIGINT, intHandler);

    key_t public_key = ftok(getenv("HOME"), PROJECT_ID);

//   int server_queue = msgget(public_key, 0);
//    if (server_queue != -1) msgctl(server_queue, IPC_RMID, NULL);
    printf("!%s\n",strerror(errno));

    server_queue = msgget(public_key, IPC_CREAT | IPC_EXCL|0777);
    if (server_queue == -1) FAILURE_EXIT("server_queue wasn't created: %s", strerror(errno));
    printf("%d\n",server_queue);


    int k = msgrcv(server_queue,&msg,sizeof(msg)-sizeof(msg.type),0,0);
    printf("-->%d\n,",k);
    printf("!%s\n",strerror(errno));
    printf("%s\n",msg.text);





    if(msgctl(server_queue,IPC_RMID,NULL)== -1) FAILURE_EXIT("Couldn't delete server queue: %s\n",strerror(errno) );
    return 0;
}

