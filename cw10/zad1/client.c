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
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(9991);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_socket,(struct sockaddr*) &address, sizeof(address)) == -1) FAILURE_EXIT("Failed in connecting to server_socket\n");

    char buf[256]="ala ma psa\n";
    char buf1[256]="kot ma psa\n";
    Msg msg;
    msg.type = MUL;
    msg.arg1 = 1;
    msg.arg2 = 4;
    
       
    write(client_socket,&msg,sizeof(Msg));
    
    
    
    sleep(10);
    

    close(client_socket);

    return 0;
}
