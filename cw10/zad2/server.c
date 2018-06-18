#include "common.h"

#define MAX_EVENTS 10
#define MAX_CLIENTS 20

#define WRITE(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}

typedef struct{
    int is_active;
    struct sockaddr msg_addr;
    char name[MAX_ARRAY];
    int ponged;
    int fd;
    socklen_t addrsize;
    
}Client;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t ping_mutex = PTHREAD_MUTEX_INITIALIZER; 


Client client[MAX_CLIENTS];
int nclients=0;

//file descriptors
int web_fd, local_fd, epoll;
struct sockaddr_in server_addr;
struct sockaddr_un local_address;

Msg msg;
struct sockaddr msg_addr;
socklen_t addrsize;


pthread_t threads[3];
char unix_path[MAX_ARRAY];
int port_number;

void __del__();
void __init__(int argc, char*argv[]);
void registerClient();
void receiveMessage(int fd);
void *handleTerminal(void * arg);


void signalHandler(){
    exit(0);
}

void eraseClient(int i){
    
    strcpy(client[i].name,"unknown");
    client[i].is_active =0;
    client[i].ponged =1;
    client[i].fd =-1;

}

void removeSocket(int fd,struct sockaddr address,socklen_t addrsize){
    
    msg.type = KILL_CLIENT;
    sendto(fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr, addrsize);
  
}

///etc/init.d/networking restart




int main(int argc, char *argv[]){
    __init__(argc,argv);

    printf("Server starts loop.\n");

    while(1){
       
        struct epoll_event event;
        int nfd = epoll_wait(epoll,&event,1,-1);
   
        if(event.data.fd == web_fd || event.data.fd == local_fd){
            
            receiveMessage(event.data.fd);

        }else{
            WRITE("WHAT?\n");
        }

       
    }
    

}

void *pingClients(void * arg){
    while(1){
        
        pthread_mutex_lock(&ping_mutex);
        for(int i=0;i<MAX_CLIENTS;i++){
            if(client[i].is_active){
                client[i].ponged =0;
                Msg msg;
                msg.type =PING;
                sendto(client[i].fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr, client[i].addrsize);

            }
        }
        pthread_mutex_unlock(&ping_mutex);

        sleep(5);
        
        pthread_mutex_lock(&ping_mutex);        
        for(int i=0;i<MAX_CLIENTS;i++){
            if(client[i].is_active && client[i].ponged ==0){
                removeSocket(client[i].fd,client[i].msg_addr,client[i].addrsize);
                eraseClient(i);
            }
        }
        pthread_mutex_unlock(&ping_mutex);

    }
    
}


void receiveMessage(int fd){
    
    
    
    recvfrom(fd,&msg,sizeof(Msg),0 ,&msg_addr, &addrsize);    
    
    WRITE("-> %i\n",msg.type);
    switch(msg.type){
       
        case UNREGISTER:
            
            pthread_mutex_lock(&mutex);
            int flag =0;
            for(int i=0;i<MAX_CLIENTS;i++){
                
                if(client[i].is_active && strcmp(client[i].name,msg.name)==0){
                    flag=1;
                    nclients--;
                    removeSocket(fd,client[i].msg_addr,client[i].addrsize);
                    eraseClient(i);
                    
                    break;
                }
            }
            if(!flag) FAILURE_EXIT("Client couldn't unregister magic???\n");

            pthread_mutex_unlock(&mutex);
            break;
        case REGISTER:
            
            
            pthread_mutex_lock(&mutex);
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client[i].is_active && strcmp(client[i].name,msg.name)==0){
                    
                    WRITE("Client name exists, kill client\n");
                    msg.type = KILL_CLIENT;
                    sendto(fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr, addrsize);
                    
                    pthread_mutex_unlock(&mutex);
                    return;
                }
            }

            int client_registered_successfully = 0;
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client[i].is_active == 0){
                    client[i].msg_addr = msg_addr;
                    client[i].is_active=1;
                    client[i].ponged=1;
                    client[i].fd = fd;
                    client[i].addrsize = addrsize;
                    strcpy(client[i].name,msg.name);
                    nclients++;
                    WRITE("Client: %s registered successfully\n",client[i].name);
                    client_registered_successfully=1;
              
                    msg.type =SUCCESS;
                    sendto(fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr, addrsize);
                    
                    break;
                }   
            }
            if(!client_registered_successfully) WRITE("Client wasn't registered\n");
            pthread_mutex_unlock(&mutex);

            break;
        case RESULT:
            WRITE("result: %i\n",msg.result);
            break;

        case PONG:
            pthread_mutex_lock(&mutex);
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client[i].is_active && client[i].fd == fd){
                    client[i].ponged =1;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
            break;

        default:
            WRITE("Jak sie ciesze ze nie ma wiecej zestawow\n");
    }
    
}




void *handleTerminal(void * arg){
    while(1){
        
        Msg msg;
        char type;
       
        scanf(" %c %i %i",&type,&msg.arg1,&msg.arg2);
        WRITE("jeek\n");
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



        WRITE("J\n");
        int who = rand()%nclients;
        int k=0;
        for(int i=0;i<MAX_CLIENTS;i++){
            if(client[i].is_active){
                if(k == who){
                    
                    sendto(client[i].fd, (const void *)&msg, sizeof(Msg),0, &client[i].msg_addr,sizeof(struct sockaddr));

                    break;
                }
                k++;
            }
        } 
        pthread_mutex_unlock(&mutex);
    }
}
void howToUse(){
    printf("./server 9992 ./myfile\n");
    exit(0);
}

void __init__(int argc, char *argv[]){
    if(argc!=3) howToUse();
    strcpy(unix_path,argv[2]);
    port_number = atoi(argv[1]);
    if(port_number<100 || port_number > 65535 ) howToUse();

    signal(SIGINT,signalHandler);
    atexit(__del__);
    srand(time(NULL));
    
    //web socket ---------------------------------------------------------------------------
    if((web_fd = socket(AF_INET, SOCK_DGRAM,0)) == -1) FAILURE_EXIT("Failed to create communication endpoint web_fd\n");
    int yes=1;
    if (setsockopt(web_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) == -1) FAILURE_EXIT("setsockopt web_fd\n");
    
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9992);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(web_fd,(const struct sockaddr*) &server_addr,sizeof(struct sockaddr)) == -1) FAILURE_EXIT("Failed to assign server_addr to a web_fd: %s\n",strerror(errno));



    //Local socket -----------------------------------------------------------------------------
     if((local_fd = socket(AF_UNIX, SOCK_DGRAM,0)) == -1) FAILURE_EXIT("Failed to create communication endpoint local_fd\n");
    int yes1=1;
    if (setsockopt(web_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes1)) == -1) FAILURE_EXIT("setsockopt local");
    remove(unix_path);
    local_address.sun_family = AF_UNIX;  
    strcpy(local_address.sun_path,unix_path) ;
 
    if(bind(local_fd,(const struct sockaddr*) &local_address,sizeof(struct sockaddr_un)) == -1) FAILURE_EXIT("Failed to assign server_addr to a local_fd: %s\n",strerror(errno));
   
    //------------------------------------

    epoll = epoll_create1(0);
    if(epoll == -1) FAILURE_EXIT("Failed to create new epoll instance: %s\n",strerror(errno));
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = web_fd;
    if(epoll_ctl(epoll,EPOLL_CTL_ADD,web_fd,&event)== -1) FAILURE_EXIT("Failed to register web_fd file descriptor on epoll instance:   %s\n",strerror(errno));

    event.data.fd = local_fd;
    if(epoll_ctl(epoll,EPOLL_CTL_ADD,local_fd,&event)== -1) FAILURE_EXIT("Failed to register local_fd file descriptor on epoll instance:   %s\n",strerror(errno));

   

    for(int i=0;i<MAX_CLIENTS;i++){
        eraseClient(i);
    }

    pthread_create(&threads[0],NULL,handleTerminal,NULL);
    pthread_create(&threads[1],NULL,pingClients,NULL);
}


void __del__(){
   
    for(int i=0;i<MAX_CLIENTS;i++){
        if(client[i].is_active){
            Msg msg;
            msg.type = KILL_CLIENT;
            sendto(client[i].fd, (const void *)&msg, sizeof(Msg),0, &client[i].msg_addr,sizeof(struct sockaddr));

            eraseClient(i);
        }
    }
    close(web_fd);
    close(local_fd);
    remove(unix_path);
}