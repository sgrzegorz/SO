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
#define maximum_number_of_children 1000

void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}



volatile int N; //final number of children
volatile int K; //
volatile int n; // number of existing children
volatile int k; //number of received requests
volatile int **children;
//children[X][0] child pid
//children[X][1] if request from child was received
//children[X][2] if permission was sent
//children[X][3] number of received real-time signal
//children[X][4] value returned by child

void sigchldHandler(int signo, siginfo_t* info, void* context);
void requestHandler(int signo, siginfo_t* info, void* context);
void printA();
void realTimeHandler(int signo, siginfo_t* info, void* context);
void parentExitHandler(int signo, siginfo_t* info, void* context);
void parseCommandLineArguments(int argc, char *argv[]);


// N-number of children, K-when process will get K requests it'll send singnal to children
int main(int argc,char *argv[]) {
    parseCommandLineArguments(argc,argv);
    n = k =0;


    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER|SA_SIGINFO;

    act.sa_sigaction = sigchldHandler;
    if(sigaction(SIGCHLD,&act,NULL) == -1)  printf("Cannot catch SIGCHLD");

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



    for(int i=0;i<N;i++){
        pid_t pid = fork();

        if(pid == 0) {
            execl("./child", "./child", NULL);
            error("Fork error happened\n");
        }else{
            children[n++][0] = pid;
            WRITE_MSG("---->%d\n", n);
        }
    }

  //  printA();

    while (1){

      //WRITE_MSG("In loop, N:%d K:%d n: %d, k: %d\n",N,K,n,k);
     //   printA();
        sleep(1);
    }

}

pid_t getChild(pid_t pid){
    for(int i=0;i<N;i++){
        if(children[i][0] == pid) return i;
    }
    return -1;
}

void rmChild(pid_t pid){
    for(int i=0;i<N;i++){
        if(children[i][0] == pid){
            children[i][0] = -1;
            return;
        }
    }


}

void sigchldHandler(int signo, siginfo_t* info, void* context){
    WRITE_MSG("||---->%d\n", n);
    if(info->si_code == CLD_EXITED ){
        children[getChild(info->si_pid)][4] = info->si_status;
        WRITE_MSG("Child: %d returned: %d \n",info->si_pid,info->si_status);
    }else{
        children[getChild(info->si_pid)][4] = -1;
        WRITE_MSG("Child %d killed by signal\n",info->si_pid);
    }

    n = n-1;
    WRITE_MSG("|---->%d\n", n);
    if(n == 0){
        WRITE_MSG("Parent dies\n");
        exit(0);
    }
    rmChild(info->si_status);
}


void requestHandler(int signo, siginfo_t* info, void* context){
    //WRITE_MSG("|||---->%d\n", n);
    //WRITE_MSG("%d\n",(int) getChild(info->si_pid));
    if(getChild(info->si_pid) == -1) return;
    WRITE_MSG("Father received request from child: %d\n",info->si_pid);
    children[getChild(info->si_pid)][1] = 1;
    k=k+1;

    if( k > K){
        children[getChild(info->si_pid)][2] = 1;
        kill(info->si_pid,SIGUSR1);

        waitpid(info->si_pid,NULL,0);

    }else if(k == K){
        //WRITE_MSG("---------------------->%d %d\n",K , k);
        for(int i=0;i<N;i++){
            if(children[i][1] == 1 && children[i][0] != -1  ){ //if asked for permission and they exists
                children[i][2] = 1;
                kill(children[i][0],SIGUSR1);

                waitpid(info->si_pid,NULL,0);

            }
        }
    }
    //WRITE_MSG("-REGGG--->%d\n", n);
}

void realTimeHandler(int signo, siginfo_t* info, void* context){
    if(getChild(info->si_pid) == -1) return;
    children[getChild(info->si_pid)][3] =  signo -SIGRTMIN;
    WRITE_MSG("RT signal received %d\n",signo -SIGRTMIN);
}

void parentExitHandler(int signo, siginfo_t* info, void* context){
    for(int i=0;i<N;i++){
        pid_t pid = (pid_t) children[i][0];
        if(pid != -1)  kill(pid,SIGKILL);
    }
    WRITE_MSG("Parent dies SIGINT\n");
    exit(0);
}

//--------------------------------------------------------------------------------------------------------------------//

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

