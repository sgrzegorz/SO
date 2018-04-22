#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define WRITE_MSG(format,...) {char buffer[255];sprintf(buffer,format, ##__VA_ARGS__);write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(code, format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(code);}



int main(int argc,char *argv[]) {
    if (argc != 3){
        WRITE_MSG("Slave parse error\n");
        exit(-1);
    }

    WRITE_MSG("I live to serve you: %d \n",getpid());
    int slaves_loop=atoi(argv[2]);
    srand((unsigned int) getpid()+ time(NULL));
    FILE *fd;
    if((fd = fopen(argv[1], "r") == NULL ) WRITE_MSG("Slave %d couldn't open fifo\n",getpid());
    WRITE_MSG("%s\n","oal");
    char buf[50];
    for(int i=0;i<slaves_loop;i++){
       char buf[50];
       FILE *f = popen("date","r");
       fgets(buf,sizeof(buf),f);
       fclose(f);
       WRITE_MSG("%s\n",buf);
       if(fwrite(buf,1,strlen(buf),fd)!=strlen(buf)) WRITE_MSG("Slave couldn't write to pipe\n");
       int length_of_sleeping = (rand() % 3000000)+2000000;
       usleep(length_of_sleeping);
   }

    fclose(fd);
    WRITE_MSG("Slave died...\n");


    return 0;
}



