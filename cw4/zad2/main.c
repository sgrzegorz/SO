#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define maximum_number_of_children 1000

void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}



void ps(char *s){
    char buff1[40];
    sprintf(buff1,s);
    strcat(buff1,"\n");
    write(1,buff1,strlen(buff1));
}



volatile int N; //final number of children
volatile int K; //
volatile int n; // number of existing children
volatile int k; //number of received requests
volatile int **children;
volatile int *signal_queue;
//children[X][0] child pid
//children[X][1] if request from child was received
//children[X][2] if permission was sent
//children[X][3] number of received real-time signal
//children[X][4] value returned by child

void requestHandler(int signo, siginfo_t* info, void* context);
void printA();
void realTimeHandler(int signo, siginfo_t* info, void* context);
void parentExitHandler(int signo, siginfo_t* info, void* context);
void parseCommandLineArguments(int argc, char *argv[]);

// N-number of children, K-when process will get K requests it'll send singnal to children
int main(int argc,char *argv[]) {
    parseCommandLineArguments(argc,argv);
    int n = k =0;

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER|SA_SIGINFO;

    act.sa_sigaction = requestHandler;
    if(sigaction(SIGUSR1,&act,NULL) == -1) printf("Cannot catch requestHandler");

    act.sa_sigaction = parentExitHandler;
    if(sigaction(SIGINT,&act,NULL) == -1)  printf("Cannot catch SIGINT");

    act.sa_sigaction = realTimeHandler;
    for(int i=SIGRTMIN;i<=SIGRTMAX;i++){
        if(sigaction(i,&act,NULL) == -1) printf("Ehh");
    }


    children = calloc(N,sizeof(int*));
    for(int i=0;i<N;i++){
        children[i] = calloc(5,sizeof(int));
    }
    signal_queue = calloc(N,sizeof(int));



    for(int i=0;i<N;i++){
        pid_t pid = fork();

        if(pid == 0) {
            execl("./child", "./child", NULL);
            error("Fork error happened\n");
        }else{
            children[n++][0] = pid;
        }
    }



    while (1){
        //char buffer[30];
        //sprintf(buffer,"In loop, N:%d K:%d n: %d, k: %d\n",N,K,n,k);
        //write(1, buffer, strlen(buffer));
        WRITE_MSG("In loop, N:%d K:%d n: %d, k: %d\n",N,K,n,k);
      //  printA();
        sleep(1);
    }
//    while(wait(NULL)){
//        sleep(1);
//    }

    printf("here comes the rain\n");

    return 0;
}


void requestHandler(int signo, siginfo_t* info, void* context){
    char buffer[100];
    sprintf(buffer, "Father received request from child: %d\n",info->si_pid);
    write(1, buffer, strlen(buffer));

    int index = -1;
    for(int i=0;i<N;i++){

        if(children[i][0] == 0){
          //  sprintf(buffer, "1");
         //   write(1, buffer, strlen(buffer));
            children[i][0] = info->si_pid;
            children[i][1] = 1;
            k++;
            break;
        }
    }

    if( k > K){
       // children[index][2] =1; //permission was sent
       // sprintf(buffer, "2");
      //  write(1, buffer, strlen(buffer));
        kill(info->si_pid,SIGUSR1);
        int status;
        waitpid(info->si_pid,&status,0);
        if(WIFEXITED(status)){
            children[index][4] = WEXITSTATUS(status); //returned value
        }

    }else if(k == K){
       // sprintf(buffer, "3");
       // write(1, buffer, strlen(buffer));
        for(int i=0;i<N;i++){
            //permission was sent
            if(children[i][1] == 1){

                kill(children[i][0],SIGUSR1);
                children[i][2] = 1;
                int status;
                waitpid(info->si_pid,&status,0);

                if(WIFEXITED(status)) children[i][4] = WEXITSTATUS(status);
            }
        }
    }
}

void realTimeHandler(int signo, siginfo_t* info, void* context){
    char buffer[100];
    for(int i=0;i<N;i++){
        if(children[i][0] == info->si_pid){
            children[i][3] = signo -SIGRTMIN;
            WRITE_MSG("RT signal received %d\n",children[i][3]);
            break;
        }
    }
}

void parentExitHandler(int signo, siginfo_t* info, void* context){
    for(int i=0;i<N;i++){
        kill(children[i][0],SIGINT);
    }
    exit(0);
}


void printA(){
    char buff[2000];
    sprintf(buff + strlen(buff),"\n");
    for(int j=0;j<5;j++){
        sprintf(buff +strlen(buff),"\n");
        if(j==0)sprintf(buff + strlen(buff),"%20s","pid");
        if(j==1)sprintf(buff + strlen(buff),"%20s","request");
        if(j==2)sprintf(buff + strlen(buff),"%20s","permission was send?");
        if(j==3)sprintf(buff + strlen(buff),"%20s","real time signal");
        if(j==4)sprintf(buff + strlen(buff),"%20s","returned val");

        for(int i=0;i<N;i++){
            sprintf(buff + strlen(buff),"|%8d", children[i][j]);
        }
        sprintf(buff + strlen(buff),"|\n");
    }
    sprintf(buff + strlen(buff),"\n");
    write(1,buff,strlen(buff));
    buff[0] = '\0';

}

void parseCommandLineArguments(int argc, char *argv[]){
    void printInfo(){
        printf("Please use correct format of arguments: ");
        printf("./zad <limit> <number of children> \n");
        printf("./zad 35 40\n");
        exit(-1);
    }
    if(argc == 3){
        K = atoi(argv[1]);
        N = atoi(argv[2]);
        if(K > N) error("K should be smaller than N");
    }else{
        printInfo();
    }
}