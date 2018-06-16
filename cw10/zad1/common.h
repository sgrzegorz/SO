#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <signal.h>
#define MAX_ARRAY 50

typedef struct{
    int type;
    int arg1;
    int arg2;
    int result;
    char name[MAX_ARRAY];
}Msg;

enum operation{MUL=0,ADD=1,DIV=2,SUB=3,KILL_CLIENT=4,PING=5,PONG=6,RESULT=7,UNREGISTER=8,REGISTER=9,SUCCESS=10};


#define WRITE(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}


