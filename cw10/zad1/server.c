#include "common.h"

#define MAX_EVENTS 10


//file descriptors
int server_socket, client_socket, epoll;

void __del__(){
    if(shutdown(server_socket,SHUT_RDWR)) printf("Atexit failed to shutdown server_socket\n");
    close(server_socket);
}

void signalHandler(){
    exit(0);
}

void __init__(int argc, char*argv[]){
    signal(SIGINT,signalHandler);
    atexit(__del__);
}






int main(int argc, char*argv[]){
    __init__(argc,argv);

    if((server_socket = socket(AF_INET, SOCK_STREAM,0)) == -1) FAILURE_EXIT("Failed to create communication endpoint\n");

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(9888);
    address.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket,(struct sockaddr*) &address,sizeof(address)) == -1) FAILURE_EXIT("Failed to assign address to a socket: %s\n",strerror(errno));

    if(listen(server_socket,5) == -1) FAILURE_EXIT("Failed to mark server_socket as a passive socket\n");

    epoll = epoll_create1(0);
    if(epoll == -1) FAILURE_EXIT("Failed to create new epoll instance: %s\n",strerror(errno));

    struct epoll_event event1;
    event1.events = EPOLLIN;
    event1.data.fd = server_socket;
    if(epoll_ctl(epoll,EPOLL_CTL_ADD,server_socket,&event1)== -1) FAILURE_EXIT("Failed to register server_socket file descriptor on epoll instance\n");

    //Failed to extract the first connection request on the queue of pending connections
    client_socket = accept(server_socket,NULL,NULL);
    event1.events = EPOLLIN;
    event1.data.fd = client_socket;
    if(epoll_ctl(epoll,EPOLL_CTL_ADD,client_socket,&event1)== -1) FAILURE_EXIT("Failed to register server_socket file descriptor on epoll instance\n");

    printf("2111\n");

   // setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    
    while(1){
        // struct epoll_event event;
        struct epoll_event event;
        int fd = epoll_wait(epoll,&event,1,-1);
        printf("%i\n",fd);
        if(fd == -1) FAILURE_EXIT("Epoll wait failed\n");
        
        printf("Hejo\n");

        char buf[256];
        read(client_socket,buf,sizeof(buf));
        printf("%s",buf);
    }
   



    close(server_socket);

}