#include "common.h"

int socket_fd;



void __del__(){
    if(shutdown(socket_fd,SHUT_RDWR)) printf("Atexit failed to shutdown socket_fd\n");
    close(socket_fd);
}

void __init__(){
    atexit(__del__);
}




int main(){
    __init__();

    int res;
    atexit(__del__);

    socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd == -1) FAILURE_EXIT("Failed to create client socket\n");

  

    uint32_t ip = inet_addr("94.254.145.105"); // this code I get when I type "what is my ip?" in internet
    if(ip == -1) FAILURE_EXIT("Failed to convert ip address: %s\n",strerror(errno));
    uint16_t port_number = htons(9992);
    if (port_number < 1024 || port_number > 65535)  FAILURE_EXIT("Incorrect number of port\n");

    struct sockaddr_in address;
    memset(&address,'\0',sizeof(address));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = port_number;
    
   
    printf("Here code stops...\n");
    res = connect(socket_fd,(const struct sockaddr*) &address, sizeof(address));
    if(res == -1) FAILURE_EXIT("Failed in connecting to server_socket: %s\n",strerror(errno));


    Msg msg;
    msg.type = MUL;
    msg.arg1 = 1;
    msg.arg2 = 4;
    write(socket_fd,&msg,sizeof(Msg));
    
    
    WRITE("Hej \n");
    while(1){
        
    }
    

    close(socket_fd);

    return 0;
}
//poprawa: 