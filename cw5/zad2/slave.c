#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

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
    FILE *fd = fopen(argv[1], "a"); if(fd == NULL) WRITE_MSG("Slave %d couldn't open fifo\n",getpid());


    for(int i=0;i<slaves_loop;i++){
       char buf[256];
        sprintf(buf, "%ld", (long) getpid());

       FILE *f = popen("date","r");
       fgets(buf+strlen(buf),sizeof(buf),f);
       fclose(f);

       if(fwrite(buf,1,strlen(buf),fd)!=strlen(buf)) WRITE_MSG("Slave couldn't write to pipe\n");
       int length_of_sleeping = (rand() % 3000000)+2000000;
       usleep(length_of_sleeping);
   }

    fclose(fd);
    WRITE_MSG("Slave died...\n");


    return 0;
}



