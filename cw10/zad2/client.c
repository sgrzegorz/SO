#include "common.h"

int socket_fd;
char name[MAX_ARRAY];
char *ip;
int port;
char *path;
struct sockaddr_in msg_addr;
Msg msg;

void __del__();
void howToUse();
void sigintHandler(){ exit(0);}
void __init__(int argc, char *argv[]);

void registerOnServer(){
    
    strcpy(msg.name,name);
    msg.type = REGISTER;

    if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto1 %s\n",strerror(errno));    

 
    if(recvfrom(socket_fd,&msg,sizeof(Msg),0 ,0,0) !=sizeof(Msg)) WRITE("recvform\n");  
    WRITE("f %s\n",msg.name);

    switch(msg.type){
        case(KILL_CLIENT):
            
            if(shutdown(socket_fd,SHUT_RDWR)) printf("Atexit failed to shutdown socket_fd\n");
            // close(socket_fd);
            exit(0);
            break;
        case(SUCCESS):
            atexit(__del__);
            break;
    }
}


int main(int argc, char *argv[]){
    
    __init__(argc, argv);
    registerOnServer();
    
    WRITE("I registered\n");
    while(1){
        
        Msg msg;
        read(socket_fd,&msg,sizeof(Msg));
        if(msg.type!=PING) WRITE("Msg received\n");
         
       
        switch(msg.type){
            case MUL:
                
                msg.type = RESULT;
                msg.result = msg.arg1 * msg.arg2;
                if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");    

                break;
            case ADD:
                
                msg.type = RESULT;
                msg.result = msg.arg1 + msg.arg2;
                if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");    

                break;

            case DIV:
                
                msg.type = RESULT;
                msg.result = msg.arg1 / msg.arg2;
                if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");    

                break;

            case SUB:
                         
                msg.type = RESULT;
                msg.result = msg.arg1 - msg.arg2;
                if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");    

                break;

            case KILL_CLIENT:
                exit(0);
                break;

            case PING:
                msg.type = PONG;
                if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");    

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
    if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");    

    
    // if(shutdown(socket_fd,SHUT_RDWR)) printf("Atexit failed to shutdown socket_fd\n");
    // close(socket_fd);
}



void howToUse(){
    printf("./client <name> <lan> <ipv4> <port>  or ./client <name> <unix> <path> \n");
    printf("./client 1 lan 94.254.145.105 9992\n");
    printf("./client 1 unix ./myfile\n");
    exit(0);
}
void __init__(int argc, char *argv[]){
    signal(SIGINT,sigintHandler);
    
    if(strcmp(argv[2],"lan")==0 && argc == 5){
        strcpy(name,argv[1]);
        port = atoi(argv[4]);
        
        int res;

        uint32_t ip = inet_addr(argv[3]); // this code I get when I type "what is my ip?" in internet
        if(ip == -1) FAILURE_EXIT("Failed to convert ip address: %s\n",strerror(errno));
        uint16_t port_number = htons(port);
        if (port_number < 1024 || port_number > 65535)  FAILURE_EXIT("Incorrect number of port\n");

       
        msg_addr.sin_family = AF_INET;    
        msg_addr.sin_addr.s_addr =INADDR_ANY;
        msg_addr.sin_port = htons(9992);

        socket_fd = socket(AF_INET, SOCK_DGRAM,0);
        if(socket_fd == -1) FAILURE_EXIT("Failed to create client socket\n");

        if(connect(socket_fd, (const struct sockaddr*) &msg_addr, sizeof(struct sockaddr)) == -1) FAILURE_EXIT("Failed to assign server_addr to a web_fd: %s\n",strerror(errno));
        
    }else if(strcmp(argv[2],"unix")==0 && argc == 4){
        strcpy(name,argv[1]);
        path = argv[3];

        struct sockaddr_un local_address;
        local_address.sun_family = AF_UNIX;
        strcpy(local_address.sun_path,path);

        socket_fd = socket(AF_UNIX, SOCK_DGRAM,0);
        if(socket_fd == -1) FAILURE_EXIT("Failed to create client socket\n");

        int res = connect(socket_fd,(const struct sockaddr*) &local_address, sizeof(local_address));
        if(res == -1) FAILURE_EXIT("Failed in connecting to server_socket unix: %s\n",strerror(errno));
      



        
    }else{
        howToUse();
    }
    


    
}