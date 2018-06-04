#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <math.h>
#define _GNU_SOURCE
#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define max(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(c, d) \
   ({ __typeof__ (c) _c = (c); \
       __typeof__ (d) _d = (d); \
     _c > _d ? _d : _c; })

char picture_name[50], max_grey_number[50];
int W,H,C;
int **I; //picture
double **K; //filtr
int **J; //new picture
int number_of_threads;
pthread_t *threads;
FILE *picture,*filter,*new_picture;
char * picture_path, *filter_path, *new_picture_path;
//convert dog.jpg -compress none dog.pgm
void printInfo();

void readFromPictureFile(){
    picture = fopen(picture_path,"r");
    if (picture == NULL){
        printInfo();
        FAILURE_EXIT("Opening picture failed: %s\n",strerror(errno));
    } 
    

    fscanf(picture,"%s",picture_name);
    fscanf(picture,"%i %i",&W,&H);
    fscanf(picture,"%s",max_grey_number);
    
    I = calloc(H,sizeof(int*));
    J = calloc(H,sizeof(int*));
    for(int h=0;h<H;h++){        
        I[h] = calloc(W,sizeof(int));
        J[h] = calloc(W,sizeof(int));
    }
    
    for(int h=0;h<H;h++){
        for(int w=0;w<W;w++){           
            fscanf(picture,"%i",&I[h][w]);
        }
    }
    fclose(picture);
}

void readFromFilterFile(){
    filter = fopen(filter_path,"r");
    if (filter == NULL){
        printInfo();
        FAILURE_EXIT("Opening filter failed: %s\n",strerror(errno));
    }

    fscanf(filter,"%i",&C);
    K = calloc(C,sizeof(double*));
    for(int w=0;w<W;w++){        
        K[w] = calloc(C,sizeof(double));
    }
    for(int h=0;h<C;h++){
        for(int w=0;w<C;w++){           
            fscanf(filter,"%d",&I[h][w]);
        }
    }  
    fclose(filter);
}

int getNewValue(int H_PIXEL, int W_PIXEL){
    double sum=0;
    for(double h=0;h<C;h++){
        int h_index = (int)max(1,H_PIXEL-ceil((double) C/2)+h);
        for(double w=0;w<C;w++){
            int w_index = (int)max(1,W_PIXEL-ceil((double) C/2)+w);
            sum+=I[h_index][w_index]*K[(int)h][(int)w];
        }
    }
    return (int) round(sum);
}

void *filterPicture(void* thread_i){
    int thread_index = (intptr_t) thread_i;
    printf(" %i \n",thread_index);
    int start_column = H/number_of_threads*thread_index;
    int next_start_column = H/number_of_threads*(thread_index+1);
    if(thread_index ==number_of_threads-1)  next_start_column = H;
    for(int h=start_column;h<next_start_column;h++){
        for(int w=0;w<W;w++){
            if(w==0 || h ==0 || w ==W-1 || h == H-1) continue;
            J[h][w] = getNewValue(h,w);
        }
        
    }
    return NULL;
}

void saveResults(){
    
    new_picture = fopen(new_picture_path,"w");
    if(new_picture == NULL){
        printInfo();
        FAILURE_EXIT("Failed to save new picture: %s\n",strerror(errno));
    } 
    fprintf(new_picture,"%s\n",picture_name);
    fprintf(new_picture,"%i %i\n",W,H);
    fprintf(new_picture,"%s\n",max_grey_number);

    for(int h=0;h<H;h++){
        for(int w=0;w<W;w++){
            if(w!=W-1){
                fprintf(new_picture,"%i ",J[h][w]);
            }else{
                fprintf(new_picture,"%i\n",J[h][w]);
            }   
        }
    }


    fclose(new_picture);
}

void printInfo(){
    printf("Please enter correct arguments:\n");
    printf("./zad <number of threads> <picture path> <filter path> <output path>\n");
    printf("./zad 5 ../dog.pgm ../edge_detection.txt ../newdog.pgm\n");
}

void parseCommandArgs(int argc, char * argv[]){
    if(argc !=5) printInfo();
    number_of_threads = atoi(argv[1]);
    picture_path = argv[2];
    filter_path = argv[3];
    new_picture_path = argv[4];

}


int main(int argc, char * argv[]){
    parseCommandArgs(argc,argv);
    readFromPictureFile();
    readFromFilterFile();
    

    threads = calloc(number_of_threads,sizeof(pthread_t));

    for(int i=0;i<number_of_threads;i++){
        pthread_create(&threads[i],NULL,filterPicture,(void *) (intptr_t) i);
    }


    for(int i=0;i<number_of_threads;i++){
        if(pthread_join(threads[i],NULL)!=0) FAILURE_EXIT("Waiting for thread failed\n");
    }

    saveResults();
    printf("hello workd\n");
}