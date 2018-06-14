#include "common.h"

#define MAX_EVENTS 10
#define MAX_CLIENTS 4096

#define WRITE(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}

typedef struct{
    int is_active;
    int fd;
    char name[MAX_ARRAY];
}Client;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

Client client[MAX_CLIENTS];
int nclients=0;

//file descriptors
int server_fd, client_fd, epoll;
struct sockaddr_in server_addr;

pthread_t threads[3];

void __del__();
void __init__(int argc, char*argv[]);
void registerClient();
void receiveMessage(int fd);
void *handleTerminal(void * arg);


void signalHandler(){
    exit(0);
}



///etc/init.d/networking restart

void removeClient(){

}



int main(int argc, char*argv[]){
    __init__(argc,argv);
    pthread_create(&threads[0],NULL,handleTerminal,NULL);

    printf("Server starts loop.\n");

    while(1){
        // struct epoll_event event;
        struct epoll_event event;
        int nfd = epoll_wait(epoll,&event,1,-1);

        

        if(event.data.fd == server_fd){
            registerClient();
        }else{
            WRITE("me:\n")
            receiveMessage(event.data.fd);
        }
       
    }
    close(server_fd);

}


void receiveMessage(int fd){
    Msg msg;
    read(fd,&msg,sizeof(Msg));
    switch(msg.type){
        case UNREGISTER:

            pthread_mutex_lock(&mutex);
            int flag =0;
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client[i].is_active && strcmp(client[i].name,msg.name)==0){
                    client[i].is_active =0;
                    flag=1;
                    nclients--;
                    break;
                }
            }
            if(!flag) FAILURE_EXIT("Client couldn't unregister magic???\n");

            pthread_mutex_unlock(&mutex);
            break;
        case REGISTER:
            registerClient();
            break;
        case RESULT:
            WRITE("result: %i\n",msg.result);

            break;
        default:
            WRITE("Unknown msg type\n");
    }
    printf("%i %i\n",msg.arg1,msg.arg2);
}


void registerClient(){
    int new_client = accept(server_fd,NULL,NULL);
    WRITE("New client: %i registered\n",new_client);

    pthread_mutex_lock(&mutex);
    int client_registered_successfully = 0;
    for(int i=0;i<MAX_CLIENTS;i++){
        if(client[i].is_active = 0){
            client[i].fd = new_client;
            client[i].is_active=1;
            nclients++;
            WRITE("Client registered successfully\n");
            client_registered_successfully=1;
            break;
        }   
    }
    if(!client_registered_successfully) WRITE("Client wasn't registered\n");
    pthread_mutex_unlock(&mutex);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = new_client;
    if(epoll_ctl(epoll,EPOLL_CTL_ADD,new_client,&event)== -1) FAILURE_EXIT("Failed to register client on epoll: %s\n",strerror(errno));

}


void *handleTerminal(void * arg){
    while(1){
        
        Msg msg;
        char type;
       
        scanf(" %c %i %i",&type,&msg.arg1,&msg.arg2);
       
        switch(type){
            case '+':
                msg.type = ADD;
                break;
            case '-':
                msg.type = SUB;
                break;
            case '/':
                msg.type = DIV;
                break;
            case '*':
                msg.type = MUL;
                break;
            default:
                WRITE("unknown command\n");     
        }
        
        pthread_mutex_lock(&mutex);

        if(nclients == 0){
            WRITE("No clients registered...\n"); 
            pthread_mutex_unlock(&mutex);
            continue;
        } 

        
        int who = rand()%nclients;
        int k=0;
        for(int i=0;i<MAX_CLIENTS;i++){
            if(client[i].is_active){
                if(k == who){
                    write(client[i].fd,&msg,sizeof(Msg));
                    break;
                }
                k++;
            }
        } 
        pthread_mutex_unlock(&mutex);
    }
}

void __init__(int argc, char*argv[]){
    signal(SIGINT,signalHandler);
    atexit(__del__);
    srand(time(NULL));
    
        signal(SIGINT,signalHandler);
    atexit(__del__);
    srand(time(NULL));
    
    server_fd = socket(AF_INET, SOCK_STREAM,0);
    if(server_fd == -1)  FAILURE_EXIT("Failed to create communication endpoint\n");

    int yes=1;
    if (setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        FAILURE_EXIT("setsockopt");
    }
    

    memset(&server_addr,'\0',sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9992);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
       
    if(bind(server_fd,(const struct sockaddr*) &server_addr,sizeof(struct sockaddr)) == -1) FAILURE_EXIT("Failed to assign server_addr to a socket: %s\n",strerror(errno));

    if(listen(server_fd,100) == -1) FAILURE_EXIT("Failed to mark server_fd as a passive socket\n");
    

    epoll = epoll_create1(0);
    if(epoll == -1) FAILURE_EXIT("Failed to create new epoll instance: %s\n",strerror(errno));

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if(epoll_ctl(epoll,EPOLL_CTL_ADD,server_fd,&event)== -1) FAILURE_EXIT("Failed to register server_fd file descriptor on epoll instance:   %s\n",strerror(errno));

}


void __del__(){
    if(shutdown(server_fd,SHUT_RDWR)) printf("Atexit failed to shutdown server_fd: %s\n",strerror(errno));
    close(server_fd);
    for(int i=0;i<MAX_CLIENTS;i++){
        if(client[i].is_active) close(client[i].fd);
    }
}