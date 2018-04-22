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
#define WRITE_MSG(format,...) {char buffer[255];sprintf(buffer,format, ##__VA_ARGS__);write(1, buffer, strlen(buffer));}
#define FAILURE_EXIT(code, format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(code);}
#define max_number_of_slaves 1000

int main(int argc,char argv[]){




    return 0;
}

