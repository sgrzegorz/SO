

// This program is an example of how to run a command such as
// ps aux | grep root | grep sbin
// using C and Unix.

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int pid;
int pipe1[2];
int pipe2[2];

void main() {
    pipe(pipe1);
    pipe(pipe2);


    // fork (ps aux)
    if ((pid = fork()) == -1) {
        perror("bad fork1");
        exit(1);
    } else if (pid == 0) {
        // stdin --> ps --> pipe1
        exec1();
    }


    // fork (grep root)
    if ((pid = fork()) == -1) {
        perror("bad fork2");
        exit(1);
    } else if (pid == 0) {
        // pipe1 --> grep --> pipe2
        exec2();
    }

    if ((pid = fork()) == -1) {
        perror("bad fork3");
        exit(1);
    } else if (pid == 0) {
        // pipe2 --> grep --> stdout
        exec3();
    }
    // parent
    // parent
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    while(wait(NULL)>0);

}


void exec1() {
    // input from stdin (already done)
    // output to pipe1
  //  dup2(pipe1[1], 1);
    // close fds
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    // exec
   // execlp("ps", "ps", "aux", NULL);
    // exec didn't work, exit
    perror("bad exec ps");
    _exit(1);
}

void exec2() {
    // input from pipe1
  //  dup2(pipe1[0], 0);
    // output to pipe2
  //  dup2(pipe2[1], 1);
    // close fds
    printf("%d %d\n",pipe2[0],pipe2[1]);
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    // exec
 //   execlp("grep", "grep", "root", NULL);
    // exec didn't work, exit
    perror("bad exec grep root");
    _exit(1);
}

void exec3() {
    printf("%d %d\n",pipe2[0],pipe2[1]);
    // input from pipe2
    //dup2(pipe2[0], 0);
    // output to stdout (already done)
    // close fds
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    // exec
   // execlp("grep", "grep", "sbin", NULL);
    // exec didn't work, exit
    perror("bad exec grep sbin");
    _exit(1);
}


