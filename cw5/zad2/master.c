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


int main(int argc,char *argv[]){
    if(argc!=2){
        WRITE_MSG("MASTER parse error\n");
        exit(-1);
    }
    WRITE_MSG("Master: %d \n",getpid());


    if(mkfifo(argv[1],S_IWUSR | S_IRUSR)==-1) FAILURE_EXIT(-1,"Master failed with creating fifo\n");
    char line[MAX_BUF];
    WRITE_MSG("1\n");
    FILE* fd;
    if((fd= fopen(argv[1], "r")) == NULL) FAILURE_EXIT(-1,"Master failed with opening fifo\n");
    WRITE_MSG("1\n");
    WRITE_MSG("Ola\n");
    while(fgets(line,MAX_BUF,fd) !=NULL){

        WRITE_MSG("Received %s\n",line);
    }

    fclose(fd);
    unlink(argv[1]);
    WRITE_MSG("Master died... \n");
    return 0;
}

