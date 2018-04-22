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



int main(int argc,char argv[]) {
    srand((unsigned int) getpid()+ time(NULL));
    char string = argv[1];
    int N=argv[2];
    int fd = open(string, O_WRONLY);

    WRITE_MSG("I live to serve you: %d \n",getpid());


   for(int i=0;i<N;i++){
        int length_of_sleeping = (rand() % 3000000)+2000000;
      usleep(length_of_sleeping);
   }

    int fd;
    char * myfifo = "/tmp/myfifo";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);

    /* write "Hi" to the FIFO */

    write(fd, "Hi", sizeof("Hi"));
    close(fd);

    /* remove the FIFO */
    unlink(fd);






    printf("Hello, World!\n");
    return 0;
}



