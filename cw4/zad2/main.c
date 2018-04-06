#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define maximum_number_of_children 1000

void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}



volatile int N=2; //final number of children
volatile int K=2; //
volatile int n; // number of existing children
volatile int k; //number of received requests
volatile int **children;
//children[X][0] child pid
//children[X][1] if request from child was received
//children[X][2] if permission was sent
//children[X][3] number of received real-time signal
//children[X][4] value returned by child

void childRequestHandler(int signo, siginfo_t* info, void* context);
void printA();
void realTimeHandler(int signo, siginfo_t* info, void* context);
void killChildrenHandler(int signo, siginfo_t* info, void* context);


// N-number of children, K-when process will get K requests it'll send singnal to children
int main() {
    if(K > N) error("K should be smaller than N");


    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    act.sa_sigaction = childRequestHandler;
    if(sigaction(SIGUSR1,&act,NULL) == -1) printf("Cannot catch requestHandler");

    act.sa_sigaction = killChildrenHandler;
    if(sigaction(SIGINT,&act,NULL) == -1)  printf("Cannot catch SIGINT");

    act.sa_sigaction = realTimeHandler;
    for(int i=SIGRTMIN;i<=SIGRTMAX;i++){
        if(sigaction(i,&act,NULL) == -1) printf("Ehh");
    }


    children = calloc(N,sizeof(int*));
    for(int i=0;i<N;i++){
        children[i] = calloc(5,sizeof(int));
    }


    for(int i=0;i<N;i++){
        pid_t pid = fork();
        if(pid == 0){
            sleep(1);
            execl("./child","./child",NULL);
            exit(0);
        }else{
            children[i][0]=(int) pid;
            n++;
        }
    }

printA();
    while (wait(NULL)>0){

    }
//    while(wait(NULL)){
//        sleep(1);
//    }

    printf("here comes the rain\n");

    return 0;
}


void childRequestHandler(int signo, siginfo_t* info, void* context){
    printf("Father received request from child: %d\n",info->si_pid);
    int index = -1;
    for(int i=0;i<N;i++){
        if(children[i][0] == info->si_pid){
            children[i][1] = 1;
            k++;
            index=i;
            break;
        }
    }
    if(index == -1) return;


    if( k > K){
        printf("f1fff");
        printf("walks1\n");
        children[index][2] =1; //permission was sent
        kill(info->si_pid,SIGUSR1);
        int status;
        waitpid(info->si_pid,&status,0);
        if(WIFEXITED(status)){
            children[index][4] = WEXITSTATUS(status); //returned value
        }

    }else if(k == K){

        for(int i=0;i<N;i++){
            printf("ffff");
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
    printf("3");
}

void realTimeHandler(int signo, siginfo_t* info, void* context){
    for(int i=0;i<N;i++){
        if(children[i][0] == info->si_pid){
            printf("--->%d",signo);
            children[i][3] = signo -SIGRTMIN;
            printf("RTsignal received %d\n",children[i][3]);
            printA();
            break;
        }
    }
}

void killChildrenHandler(int signo, siginfo_t* info, void* context){
    for(int i=0;i<N;i++){
        kill(children[i][0],SIGINT);
    }
}


void printA(){
    printf("\n");
    for(int j=0;j<5;j++){
        if(j==0)printf("%20s","pid");
        if(j==1)printf("%20s","request");
        if(j==2)printf("%20s","permission was send?");
        if(j==3)printf("%20s","real time signal");
        if(j==4)printf("%20s","returned val");

        for(int i=0;i<N;i++){
            printf("|%8d", children[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}
