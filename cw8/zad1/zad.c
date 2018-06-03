#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define max(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(c, d) \
   ({ __typeof__ (c) _c = (c); \
       __typeof__ (d) _d = (d); \
     _c > _d ? _d : _c; })


int W,H,C;
int **I; //picture
double **K; //filtr
int **J; //new picture
FILE *picture,*filter;
//convert dog.jpg -compress none dog.pgm
void readFromPictureFile(){
    picture = fopen("../dog.pgm","r");
    if (picture == NULL) FAILURE_EXIT("Opening picture failed: %s\n",strerror(errno));
    
    char buff[1024];

    fscanf(picture,"%s",buff );
    fscanf(picture,"%i %i",&W,&H);
    fscanf(picture,"%s",buff);
    
    I = calloc(W,sizeof(int*));
    J = calloc(W,sizeof(int*));
    for(int w=0;w<W;w++){        
        I[w] = calloc(H,sizeof(int));
        J[w] = calloc(H,sizeof(int));
    }
    
    for(int h=0;h<H;h++){
        for(int w=0;w<W;w++){           
            fscanf(picture,"%i",&I[w][h]);
        }
    }    
}

void readFromFilterFile(){
    filter = fopen("../edge_detection.txt","r");
    if (filter == NULL) FAILURE_EXIT("Opening picture filter: %s\n",strerror(errno));

    fscanf(filter,"%i",&C);
    K = calloc(C,sizeof(double*));
    for(int w=0;w<W;w++){        
        K[w] = calloc(C,sizeof(double));
    }
    for(int h=0;h<C;h++){
        for(int w=0;w<C;w++){           
            fscanf(filter,"%d",&I[w][h]);
        }
    }  
}

int getNewValue(int x, int y){
    double sum=0;
    for(double i=0;i<C;i++){
        for(double j=0;j<C;j++){
            sum+=I[(int)max(1,x-C/2+i)][(int)max(1,y-C/2+j)]*K[(int)i][(int)j];
        }
    }
    return (int) round(sum);
}

int filterThePicture(){
    

}

void saveResults(){

}


int main(int argc, char * argv[]){
    readFromPictureFile();
    readFromFilterFile();



    printf("hello workd\n");
}