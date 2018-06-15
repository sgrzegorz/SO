#include "common.h"

int socket_fd;
char name[MAX_ARRAY];
char *ip;
int port;
char *path;


void __del__();
void howToUse();
void sigintHandler(){ exit(0);}
void __init__(int argc, char *argv[]);

void registerOnServer(){
    Msg msg;
    strcpy(msg.name,name);
    msg.type = REGISTER;
    write(socket_fd,&msg,sizeof(Msg));

    Msg feedback;
    read(socket_fd,&feedback,sizeof(Msg));

    switch(feedback.type){
        case(KILL_CLIENT):
            WRITE("!\n");
            if(shutdown(socket_fd,SHUT_RDWR)) printf("Atexit failed to shutdown socket_fd\n");
            close(socket_fd);
            exit(0);
            break;
        case(SUCCESS):
            atexit(__del__);
            break;
    }
}


int main(int argc, char *argv[]){
    WRITE("I start\n");
    __init__(argc, argv);
    registerOnServer();
    
   
    while(1){
        
        Msg msg;
        read(socket_fd,&msg,sizeof(Msg));
        WRITE("Msg received\n");
         
        Msg feedback;
        switch(msg.type){
            case MUL:
                
                feedback.type = RESULT;
                feedback.result = msg.arg1 * msg.arg2;
                write(socket_fd,&feedback,sizeof(feedback));
                break;
            case ADD:
                
                feedback.type = RESULT;
                feedback.result = msg.arg1 + msg.arg2;
                write(socket_fd,&feedback,sizeof(feedback));
                break;

            case DIV:
                
                feedback.type = RESULT;
                feedback.result = msg.arg1 / msg.arg2;
                write(socket_fd,&feedback,sizeof(feedback));
                break;

            case SUB:
                         
                feedback.type = RESULT;
                feedback.result = msg.arg1 - msg.arg2;
                write(socket_fd,&feedback,sizeof(feedback));
                break;

            case KILL_CLIENT:
                exit(0);
                break;

            case PING:
                feedback.type = PONG;
                write(socket_fd,&feedback,sizeof(feedback));
                break;
            default:
                //WRITE("Unknown message type\n");
                exit(0);
        }
    }

}



void __del__(){
    Msg msg;
    strcpy(msg.name,name);
    msg.type = UNREGISTER;
    write(socket_fd,&msg,sizeof(Msg));
    if(shutdown(socket_fd,SHUT_RDWR)) printf("Atexit failed to shutdown socket_fd\n");
    close(socket_fd);
}



void howToUse(){
    printf("./client <name> <lan> <ipv4> <port>  or ./client <name> <unix> <path> \n");
    printf("./client 1 lan 94.254.145.105 9992\n");
    exit(0);
}
void __init__(int argc, char *argv[]){
    if(argc == 5){
        strcpy(name,argv[1]);
        port = atoi(argv[4]);

    }else if(argc ==4){
        strcpy(name,argv[1]);
        path = argv[3];

    }else{
        howToUse();
    }
    
    if(strcmp(argv[2],"lan")==0){
        signal(SIGINT,sigintHandler);
        int res;
        socket_fd = socket(AF_INET, SOCK_STREAM,0);
        if(socket_fd == -1) FAILURE_EXIT("Failed to create client socket\n");

        uint32_t ip = inet_addr(argv[3]); // this code I get when I type "what is my ip?" in internet
        if(ip == -1) FAILURE_EXIT("Failed to convert ip address: %s\n",strerror(errno));
        uint16_t port_number = htons(port);
        if (port_number < 1024 || port_number > 65535)  FAILURE_EXIT("Incorrect number of port\n");

        struct sockaddr_in address;
        memset(&address,'\0',sizeof(address));
        
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = port_number;


        
       
        res = connect(socket_fd,(const struct sockaddr*) &address, sizeof(address));
        if(res == -1) FAILURE_EXIT("Failed in connecting to server_socket: %s\n",strerror(errno));
        WRITE("2\n");
    }else if(strcmp(argv[2],"unix")==0){
        
    }else{
        howToUse();
    }
    


    
}