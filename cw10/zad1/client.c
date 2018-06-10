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
    address.sin_port = htons(9888);
    address.sin_addr.s_addr = INADDR_ANY;

    if(connect(client_socket,(struct sockaddr*) &address, sizeof(address)) == -1) FAILURE_EXIT("Failed in connecting to server_socket\n");

    char buf[256]="ala ma psa\n";
    char buf1[256]="kot ma psa\n";
    while(1){
       
        write(client_socket,&buf,sizeof(buf));
        write(client_socket,&buf1,sizeof(buf1));
    }
    
    sleep(4);
    

    close(client_socket);

    return 0;
}
