#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#define maximum_number_of_children 1000

void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}



volatile int N=5; //final number of children
volatile int K=3; //
volatile int n; // number of existing children
volatile int k; //number of received requests
volatile int **children;
//children[X][0] child pid
//children[X][1] if request from child was received
//children[X][2] if permission was sent
//children[X][3] number of received real-time signal
//children[X][4] value returned by child

void printA(){
    printf("\n");
    for(int j=0;j<4;j++){
        if(j==0)printf("%15s","pid");
        if(j==1)printf("%15s","request");
        if(j==2)printf("%15s","sig received");
        if(j==3)printf("%15s","returned val");

        for(int i=0;i<N;i++){
            printf("|%8d", children[i][j]);
        } 
        printf("|\n");
    }
    printf("\n");
}


void childRequestHandler(int signo, siginfo_t* info, void* context){

    int isMyChild=0;
    int index;
    for(int i=0;i<N;i++){
        if(children[i][0] == info->si_pid){
            children[i][1] = 1;
            isMyChild =1;
            index=i;
            k++;
            break;
        }
    }
    if(!isMyChild) return;
    if( k > K){
        children[index][2] =1; //permission was sent
        kill(info->si_pid,SIGUSR1);
        int status;
        waitpid(info->si_pid,&status,0);
        if(WIFEXITED(status)){
            children[index][4] = WEXITSTATUS(status); //returned value
        }


    }else if(k == K ){
        for(int i=0;i<N;i++){
            kill(children[i][0],SIGUSR1);
            children[i][2] = 1; //permission was sent
            int status;
            waitpid(info->si_pid,&status,0);
            if(WIFEXITED(status)){
                children[index][4] = WEXITSTATUS(status); //returned value
            }

        }
    }
}

void childSentRealTimeSignalHandler(int signo, siginfo_t* info, void* context){
    for(int i=0;i<N;i++){
        if(children[i][0] == info->si_pid){
            children[i][3] = signo;
            break;
        }
    }
}

void killChildrenHandler(int signo, siginfo_t* info, void* context){
    for(int i=0;i<N;i++){
        kill(children[i][0],SIGINT);
    }
}

// N-number of children, K-when process will get K requests it'll send singnal to children
int main() {
    if(K > N) error("K should be smaller than N");


    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    act.sa_sigaction = childRequestHandler;
    sigaction(SIGUSR1,&act,NULL);

    act.sa_sigaction = killChildrenHandler;
    sigaction(SIGINT,&act,NULL);

    for(int n=0;n<32;n++){
        act.sa_sigaction = childSentRealTimeSignalHandler;
        sigaction(SIGRTMIN+n,&act,NULL);
    }



    children = calloc(N,sizeof(int*));
    for(int i=0;i<N;i++){
        children[i] = calloc(5,sizeof(int));
    }
    printA();

    for(int i=0;i<N;i++){
        pid_t pid = fork();
        if(pid == 0){
            execl("./child","./child",NULL);
            exit(0);
        }else{
            children[i][0]=(int) pid;
            n++;
        }
    }



    while(wait(NULL)){
        printA();
        sleep(1);
    }
    printf("here comes the rain\n");

    return 0;
}



