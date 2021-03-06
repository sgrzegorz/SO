#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <string.h>

void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}




volatile int L=2;
int type=1;
int RTsignal1;
int RTsignal2;




// ================================= parent =============================================//
volatile int p_sent=0;
volatile int p_received=0;
pid_t pid;

void parentCountingHandler(int signo, siginfo_t* info, void* context) {
    p_received ++;

    char buff[200];
    sprintf(buff,"Parent received %d signals...\n",p_received);
    write(1, buff, strlen(buff));
    if(p_received == L) kill(getpid(),SIGINT);
}

void parentSIGUSR1Handler(int signo, siginfo_t* info, void* context){
    p_received ++;
    char buff[200];
    sprintf(buff,"Parent received signal number %d.\n",p_received);
    write(1, buff, strlen(buff));
    if(p_received == L) kill((pid_t) getpid(),SIGINT);

    kill(pid,SIGUSR1);
    p_sent++;
}

void parentRTHandler(int signo, siginfo_t* info, void* context){
    p_received ++;
    char buff[200];
    sprintf(buff,"Parent received total %d RT signals \n",p_received);
    write(1, buff, strlen(buff));
    if(p_received == L){
        kill((pid_t) pid,SIGRTMIN+RTsignal2);
        sprintf(buff,"Parent dies from RT signal \n");
        write(1, buff, strlen(buff));
        exit(0);
    }

    kill(pid,SIGRTMIN+RTsignal1);
    p_sent++;
}


void parentExitHandler(int signo, siginfo_t* info, void* context){
    kill(pid,SIGUSR2);
    usleep(10000);
    char buff[200];
    sprintf(buff,"Parent dies.\n");
    write(1, buff, strlen(buff));
    exit(0);
}



// ================================= child =============================================//
volatile int ch_received=0;

void childSIGUSR1Handler(int signo, siginfo_t* info, void* context){
    char buff[200];
    sprintf(buff,"Child received SIGUSR1, number of received signals: %d \n",ch_received+1);
    write(1, buff, strlen(buff));
    
    kill(getppid(),SIGUSR1);
    ch_received++;
}

void childRTHandler(int signo, siginfo_t* info, void* context){
    ch_received++;

    char buff[200];
    sprintf(buff,"Child received real time signal: %d\n",signo - SIGRTMIN);
    write(1, buff, strlen(buff));
    kill(getppid(),signo);
}

void childExitHandler(int signo, siginfo_t* info, void* context){
    char buff[200];
    sprintf(buff,"Child dies.\n");
    write(1, buff, strlen(buff));
    exit(0);
}

// ================================= main =============================================//

void parseCommandLineArguments(int argc, char *argv[]){
    void printInfo(){
        printf("Please use correct format of arguments: ");
        printf("./zad <type> <L> \n");
        printf("<type> is: 1 or 2 \n");
        printf("L is number of signals to send. L >= 2\n");
        printf("./zad 1 10\n");
        printf("./zad 3 <RT signal> <RT signal>\n");
        printf("<RT signal> should be value from {0,1,...,31}\n");
        printf("./zad 3 2400 1 2\n");
        exit(-1);
    }

    if(argc == 3){
        if(!(strcmp(argv[1],"1") == 0 || strcmp(argv[1],"2") == 0)) printInfo();
        type = atoi(argv[1]);
        L = atoi(argv[2]);
        if(L < 0) printInfo();
    }else if(argc == 5){
        if(strcmp(argv[1],"3")!=0) printInfo();
        type = atoi(argv[1]);
        L = atoi(argv[2]);
        RTsignal1 = atoi(argv[3]);
        RTsignal2 = atoi(argv[4]);
        if(RTsignal1 < 0 || RTsignal1 >31 || RTsignal2 <0 || RTsignal2 > 31) printInfo();

    }else{
        printInfo();
    }




}
/*
void ps(char *s){
    char buff1[40];
    sprintf(buff1,s);
    write(1,buff1,strlen(buff1));
}
*/
int main(int argc, char *argv[]) {
    parseCommandLineArguments(argc,argv);
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER|SA_SIGINFO;

    pid = fork();
    if(pid == 0){

        sigset_t blocked;
        sigfillset(&blocked);
        sigdelset(&blocked,SIGUSR2);

        act.sa_sigaction = childExitHandler;
        if(sigaction(SIGUSR2,&act,NULL) == -1) error("childExitHandler failed");

        if(type == 1 || type == 2){

            sigdelset(&blocked,SIGUSR1);
            sigprocmask(SIG_BLOCK, &blocked,NULL);
            act.sa_sigaction = childSIGUSR1Handler;
            if(sigaction(SIGUSR1,&act,NULL) == -1) error("childSIGUSR1Handler failed");

        }else if(type ==3) {

            sigdelset(&blocked, SIGRTMIN + RTsignal1);
            sigdelset(&blocked, SIGRTMIN + RTsignal2);
            sigprocmask(SIG_BLOCK, &blocked, NULL);

            act.sa_sigaction = childRTHandler;
            if (sigaction(SIGRTMIN + RTsignal1, &act, NULL) == -1) error("childRTHandler failed.");
            act.sa_sigaction = childExitHandler;
            if (sigaction(SIGRTMIN + RTsignal2, &act, NULL) == -1) error("childRTHandler failed.");
        }

        while(1){
            sleep(5);
        }

        exit(0);
    }else{
            sleep(1);
            act.sa_sigaction = parentExitHandler;
            if(sigaction(SIGINT,&act,NULL) == -1) error("parentExitHandler failed");

        if(type ==1){

            act.sa_sigaction = parentCountingHandler;
            if(sigaction(SIGUSR1,&act,NULL) == -1) error("parentCountingHandler failed");
            
            for(int i=0;i<L;i++){
                kill(pid,SIGUSR1);
                p_sent ++;
                usleep(3000);
            }
            kill(pid,SIGINT);

        }else if(type == 2){

            act.sa_sigaction = parentSIGUSR1Handler;
            if(sigaction(SIGUSR1,&act,NULL) == -1) printf("Parent handler2 failed\n");
            if(L == 0){
                kill(getpid(),SIGINT);
            }else{
                p_sent++;
                kill(pid,SIGUSR1);
            }

        }else if(type ==3) {

            act.sa_sigaction = parentRTHandler;
            if(sigaction(SIGRTMIN + RTsignal1,&act,NULL) == -1) error("parentRTHandler failed");
            if(sigaction(SIGRTMIN + RTsignal2,&act,NULL) == -1) error("parentRTHandler failed");
            kill(pid, SIGRTMIN + RTsignal1);
            usleep(3000);


        }

        char buff[200];
        while(1){
            sprintf(buff,"In loop\n");
            write(1,buff,strlen(buff));
            sleep(1);
        }

        return 0;

    }


}