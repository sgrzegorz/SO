#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include<sys/stat.h>
#include <string.h>
#include<sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_BUF 10000

#define WRITE_MSG(format,...) {char buffer[255];sprintf(buffer,format, ##__VA_ARGS__);write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(code, format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(code);}


int main(int argc,char argv[]){
    int max_slaves =5;
    char string = "./cmake-build-debug";


    for(int i=0;i<max_slaves;i++){
        pid_t pid = fork();
        if(pid ==0){
            execl("./cmake-build-debug/slave",string,NULL);
            FAILURE_EXIT("Slave failed\n");
        }
    }

    char buf[MAX_BUF];
    char string = "./cmake-build-debug";
    int fd = open(string, O_RDONLY);
    while(1){
        read(fd, buf, MAX_BUF);
        printf("Received: %s\n", buf);
    }
    close(fd);


    return 0;
}

