#include "common.h"

#define MAX_EVENTS 10
#define MAX_CLIENTS 20

#define WRITE(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}

typedef struct{
    int is_active;
    int fd;
    char name[MAX_ARRAY];
    int ponged;
}Client;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t ping_mutex = PTHREAD_MUTEX_INITIALIZER; 


Client client[MAX_CLIENTS];
int nclients=0;

//file descriptors
int web_fd, local_fd, epoll;
struct sockaddr_in server_addr;
struct sockaddr_un local_address;

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

void removeSocket(int fd){
    Msg feedback;
    feedback.type = KILL_CLIENT;
    write(fd,&feedback,sizeof(feedback));
    if (epoll_ctl(epoll, EPOLL_CTL_DEL, fd, NULL) == -1) FAILURE_EXIT("Error : Could not remove client's socket from epoll\n");
   // if (shutdown(fd, SHUT_RDWR) == -1) FAILURE_EXIT("Could not shutdown client's socket: %s\n",strerror(errno));
    if (close(fd) == -1) FAILURE_EXIT("Error : Could not close client's socket\n");
}

///etc/init.d/networking restart




int main(int argc, char *argv[]){
    __init__(argc,argv);

    printf("Server starts loop.\n");

    while(1){
       
        struct epoll_event event;
        
        int nfd = epoll_wait(epoll,&event,1,-1);
   
        if(event.data.fd == web_fd || event.data.fd == local_fd){
            int new_client;
            if(event.data.fd == web_fd){
                new_client = accept(web_fd,NULL,NULL);
            }else if(event.data.fd == local_fd){
                new_client = accept(local_fd,NULL,NULL);
            }
            WRITE("Client accepted\n");
            struct epoll_event event1;
            event1.events = EPOLLIN;
            event1.data.fd = new_client;
             
            if(epoll_ctl(epoll,EPOLL_CTL_ADD,new_client,&event1)== -1) FAILURE_EXIT("Failed to register client on epoll: %s\n",strerror(errno));
            
        }else{
            receiveMessage(event.data.fd);
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
                write(client[i].fd,&msg,sizeof(Msg));
            }
        }
        pthread_mutex_unlock(&ping_mutex);

        sleep(5);
        
        pthread_mutex_lock(&ping_mutex);        
        for(int i=0;i<MAX_CLIENTS;i++){
            if(client[i].is_active && client[i].ponged ==0){
                removeSocket(client[i].fd);
                eraseClient(i);
            }
        }
        pthread_mutex_unlock(&ping_mutex);

    }
    
}


void receiveMessage(int fd){
    
    Msg msg;
    read(fd,&msg,sizeof(Msg));
    //WRITE("-> %i\n",msg.type);
    switch(msg.type){
       
        case UNREGISTER:
            
            pthread_mutex_lock(&mutex);
            int flag =0;
            for(int i=0;i<MAX_CLIENTS;i++){
                
                if(client[i].is_active && strcmp(client[i].name,msg.name)==0){
                    flag=1;
                    nclients--;
                    removeSocket(client[i].fd);
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
                    removeSocket(fd);
                    pthread_mutex_unlock(&mutex);
                    return;
                }
            }

            int client_registered_successfully = 0;
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client[i].is_active == 0){
                    client[i].fd = fd;
                    client[i].is_active=1;
                    client[i].ponged=1;
                    strcpy(client[i].name,msg.name);
                    nclients++;
                    WRITE("Client: %s registered successfully\n",client[i].name);
                    client_registered_successfully=1;

                    Msg feedback;
                    feedback.type =SUCCESS;
                    write(fd,&feedback,sizeof(Msg));
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
    if((web_fd = socket(AF_INET, SOCK_STREAM,0)) == -1) FAILURE_EXIT("Failed to create communication endpoint web_fd\n");

    int yes=1;
    if (setsockopt(web_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) == -1) FAILURE_EXIT("setsockopt web_fd\n");
    
    memset(&server_addr,'\0',sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
       
    if(bind(web_fd,(const struct sockaddr*) &server_addr,sizeof(struct sockaddr)) == -1) FAILURE_EXIT("Failed to assign server_addr to a web_fd: %s\n",strerror(errno));

    if(listen(web_fd,100) == -1) FAILURE_EXIT("Failed to mark web_fd as a passive socket\n");
    
    //Local socket -----------------------------------------------------------------------------
     if((local_fd = socket(AF_UNIX, SOCK_STREAM,0)) == -1) FAILURE_EXIT("Failed to create communication endpoint local_fd\n");
    int yes1=1;
    if (setsockopt(web_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes1)) == -1) FAILURE_EXIT("setsockopt local");
    remove(unix_path);
    local_address.sun_family = AF_UNIX;  
    strcpy(local_address.sun_path,unix_path) ;
 
    if(bind(local_fd,(const struct sockaddr*) &local_address,sizeof(struct sockaddr_un)) == -1) FAILURE_EXIT("Failed to assign server_addr to a local_fd: %s\n",strerror(errno));

    if(listen(local_fd,100) == -1) FAILURE_EXIT("Failed to mark local_fd as a passive socket\n");
   
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
    close(web_fd);
    close(local_fd);
    for(int i=0;i<MAX_CLIENTS;i++){
        if(client[i].is_active){
            Msg msg;
            msg.type = KILL_CLIENT;
            write(client[i].fd,&msg,sizeof(Msg));
            close(client[i].fd);
            eraseClient(i);
        }
    }
    remove(unix_path);
}