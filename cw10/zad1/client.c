#include "common.h"

int client_socket;



void __del__(){
    if(shutdown(client_socket,SHUT_RDWR)) printf("Atexit failed to shutdown client_socket\n");
    close(client_socket);
}

void __init__(){
    atexit(__del__);
}




int main(){
    __init__();

    client_socket = socket(AF_INET, SOCK_STREAM,0);
    if(client_socket == -1) FAILURE_EXIT("Failed to create client socket\n");

    uint32_t ip = inet_addr("94.254.145.105");
    if(ip == -1) WRITE("Failed to convert ip address: %s\n",strerror(errno));
    uint16_t port_number = htons(9991);
    if (port_number < 1024 || port_number > 65535)  FAILURE_EXIT("Incorrect number of port\n");

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port_number);
    address.sin_addr.s_addr = htonl(ip);
   
    WRITE("1\n");
    if(connect(client_socket,(const struct sockaddr*) &address, sizeof(address)) == -1) FAILURE_EXIT("Failed in connecting to server_socket: %s\n",strerror(errno));
    WRITE("2\n");

    Msg msg;
    msg.type = MUL;
    msg.arg1 = 1;
    msg.arg2 = 4;
    write(client_socket,&msg,sizeof(Msg));
    
    
    WRITE("Hej \n");
    sleep(10);
    

    close(client_socket);

    return 0;
}
//poprawa: 