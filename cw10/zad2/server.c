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
    client[i].ponged =-1;
    client[i].fd =-1;
    strcpy(client[i].name ,"Unknown");
    client[i].addrsize =-1;

}



///etc/init.d/networking restart




int main(int argc, char *argv[]){
    __init__(argc,argv);

    printf("Server starts loop.\n");

    while(1){
       
        struct epoll_event event;
        epoll_wait(epoll,&event,1,-1);
   
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
                if(verbose) WRITE("Delete pinging %s\n",client[i].name);
                client[i].ponged =0;
                Msg msg;
                msg.type =PING;
                if(client[i].fd ==web_fd){
                    if(sendto(client[i].fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&client[i].msg_addr, client[i].addrsize)!=sizeof(Msg)) WRITE("sendto36\n");
                }else{
                    if(write(client[i].fd,&msg,sizeof(Msg))!=sizeof(Msg)) FAILURE_EXIT("write0\n");
                }
              

            }
        }
        pthread_mutex_unlock(&ping_mutex);

        sleep(5);
        
        pthread_mutex_lock(&ping_mutex);        
        for(int i=0;i<MAX_CLIENTS;i++){
            if(client[i].is_active && client[i].ponged ==0){
                if(verbose) WRITE("Kill because not responding %s\n",client[i].name);
                //remove socket
                Msg msg;
                msg.type = KILL_CLIENT;
                for(int i=0;i<1;i++){
                    if(client[i].fd ==web_fd){
                        if(sendto(client[i].fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&client[i].msg_addr, client[i].addrsize)!=sizeof(Msg)) FAILURE_EXIT("sendto10 %s\n",strerror(errno));
                    }else{
                        if(write(client[i].fd,&msg,sizeof(Msg))!=sizeof(Msg)) FAILURE_EXIT("write10\n");
                    }
                    
                }
     
                eraseClient(i);
            }
        }
        pthread_mutex_unlock(&ping_mutex);

    }
    
}


void receiveMessage(int fd){
    Msg msg;
    struct sockaddr msg_addr;
    socklen_t addrsize = sizeof(msg_addr);//JEBANIE KURWA WAŻNA LINIJKA

    if(fd == web_fd){
        recvfrom(fd,&msg,sizeof(Msg),0 ,&msg_addr, &addrsize); 
    }else{
        if(read(fd,&msg,sizeof(Msg))!=sizeof(Msg)) FAILURE_EXIT("read!!!\n");
    }
       
    if(msg.type!=PONG)WRITE("Received message from client %s\n",msg.name);
    
    switch(msg.type){
       
        case UNREGISTER:
            
            pthread_mutex_lock(&mutex);
            int flag =0;
            for(int i=0;i<MAX_CLIENTS;i++){
                
                if(client[i].is_active && strcmp(client[i].name,msg.name)==0){
                    flag=1;
                    nclients--;

        
                    //remove socket
                    msg.type = KILL_CLIENT;
                    if(client[i].fd ==web_fd){
                         if(sendto(client[i].fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&client[i].msg_addr, client[i].addrsize)!=sizeof(Msg)) FAILURE_EXIT("sendto10 %s\n",strerror(errno));

                    }else{
                        if(write(fd,&msg,sizeof(Msg)!=sizeof(Msg))) FAILURE_EXIT("write1\n");
                    }
                
                    eraseClient(i);
                    
                    break;
                }
            }
            if(!flag) WRITE("ok\n");

            pthread_mutex_unlock(&mutex);
            break;
        case REGISTER:
            
            
            pthread_mutex_lock(&mutex);
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client[i].is_active && strcmp(client[i].name,msg.name)==0){
                    
                    WRITE("Client name exists, kill client\n");
                    msg.type = KILL_CLIENT;
                    if(fd == web_fd){
                        if(sendto(fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr, addrsize)!=sizeof(Msg)) FAILURE_EXIT("sendto2\n");
                    }else{
                        if(write(fd,&msg,sizeof(Msg)!=sizeof(Msg))) FAILURE_EXIT("write2\n");
                    }
                    
                    pthread_mutex_unlock(&mutex);
                    return;
                }
            }

            int client_registered_successfully = 0;
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client[i].is_active == 0){
                    
                    client[i].is_active=1;
                    client[i].ponged=1;
                    client[i].fd = fd;
                    client[i].addrsize = addrsize;
                    client[i].msg_addr = msg_addr;
                    strcpy(client[i].name,msg.name);
                    nclients++;
                    WRITE("Client: %s registered successfully\n",client[i].name);
                    client_registered_successfully=1;
              
                    strcpy(msg.name, "registered\n");
                    msg.type =SUCCESS;
                    if(fd ==web_fd){
                        if(sendto(fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr, addrsize)!=sizeof(Msg)) FAILURE_EXIT("sendto6 %s\n",strerror(errno));
                    }else{
                        if(write(fd,&msg,sizeof(Msg)!=sizeof(Msg))) FAILURE_EXIT("write3");
                    }
                   
                  
                    
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
            if(verbose) WRITE("Received pong %s\n",msg.name);
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client[i].is_active && strcmp(client[i].name,msg.name)==0){
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
                    if(client[i].fd == web_fd){
                        if(sendto(client[i].fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&client[i].msg_addr, client[i].addrsize)!=sizeof(Msg)) WRITE("sendto36\n");
                    }else{
                        if(write(client[i].fd,&msg,sizeof(Msg))!=sizeof(Msg)) FAILURE_EXIT("write5\n");
                        
                    }

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
            if(client[i].fd == web_fd){
                if(sendto(client[i].fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&client[i].msg_addr, client[i].addrsize)!=sizeof(Msg)) WRITE("sendto3\n");
            }else{
                if(write(client[i].fd,&msg,sizeof(Msg))!=sizeof(Msg)) FAILURE_EXIT("write6\n");
            }

            eraseClient(i);
        }
    }
    close(web_fd);
    close(local_fd);
    remove(unix_path);
}