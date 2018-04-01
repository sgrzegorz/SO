#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <zconf.h>
#include <memory.h>
#include <time.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/resource.h>


void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}

typedef struct {
    int rec_number;
    int rec_size;
    const char * path;
    const char * src_path;
    const char* dest_path;

} Records_File;

Records_File create(int rec_number, int rec_size, const char * path, const char * src_path,const char* dest_path){
    Records_File RF;
    RF.rec_number=rec_number;
    RF.rec_size =rec_size;
    RF.path = path;
    RF.src_path =src_path;
    RF.dest_path = dest_path;
    return RF;
}


typedef struct {
    struct timeval real;
    struct timeval user;
    struct timeval system;
} Time;



Time measureTime(){
    Time T;
    gettimeofday(&T.real , NULL);

    struct rusage tmp;
    getrusage(RUSAGE_SELF, &tmp);

    T.user = tmp.ru_utime;
    T.system = tmp.ru_stime;
    return T;
}

void printTimeDifference(Time T2, Time T1){

    if(T2.real.tv_usec >=T1.real.tv_usec){
        printf("real: %ld.%06ld s\n", T2.real.tv_sec - T1.real.tv_sec, T2.real.tv_usec - T1.real.tv_usec);
    }else{
        printf("real: %ld.%06ld s\n", T2.real.tv_sec - T1.real.tv_sec -1, (1000000+T2.real.tv_usec) - T1.real.tv_usec);
    }
    if(T2.user.tv_usec >=T1.user.tv_usec){
        printf("user: %ld.%06ld s\n", T2.user.tv_sec - T1.user.tv_sec, T2.user.tv_usec - T1.user.tv_usec);
    }else{
        printf("user: %ld.%06ld s\n", T2.user.tv_sec - T1.user.tv_sec-1, 1000000+T2.user.tv_usec - T1.user.tv_usec);
    }
    if(T2.system.tv_usec >=T1.system.tv_usec){
        printf("system: %ld.%06ld s\n", T2.system.tv_sec - T1.system.tv_sec, T2.system.tv_usec - T1.system.tv_usec);
    }else{
        printf("system: %ld.%06ld s\n", T2.system.tv_sec - T1.system.tv_sec-1, 1000000+T2.system.tv_usec - T1.system.tv_usec);
    }
}



char* genRandom(Records_File RF){

    int file = open("/dev/urandom", O_RDONLY);
    if (file < 0) error("Cannot open /dev/random. ");


    int * buff = malloc(sizeof(int) * RF.rec_size);
    if( read(file, buff, sizeof(int) * RF.rec_size ) <0)error("Cannot open /dev/random. ");

    char * s = malloc(sizeof(int) * RF.rec_size);
    char alphanum[] =     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < RF.rec_size; i++) {
        s[i] = alphanum[buff[i] % (sizeof(alphanum) - 1)];

    }


    free(buff);
    close(file);
    return s;


}


void generate(Records_File RF){

    int file = open(RF.path,O_RDWR| O_CREAT |O_TRUNC,0666);
    if (file < 0) error("Unable to open file1.txt");
    for(int i=0;i<RF.rec_number;i++){
        char * random_record = genRandom(RF);

        if( write (file, random_record, RF.rec_size *sizeof(char)) <0) error("Unable to write to file1.txt, record: ");

    }
    close(file);
}

char *get(Records_File RF,int file, int i){
    char *buff = malloc(sizeof(char)*RF.rec_size);
    if(lseek(file, RF.rec_size*i,SEEK_SET) ==-1) error("Cannot set pointer to record");
    if(read (file, buff, RF.rec_size *sizeof(char)) <0) error("Cannot get record");
    return buff;
}

void set(Records_File RF,int file,int i,char *record){
    if(lseek( file, RF.rec_size *i,SEEK_SET) ==-1) error("Cannot set pointer to record");
    if(write (file, record, RF.rec_size *sizeof(char)) <0) error("Cannot set record");
}


void sort(Records_File RF) {
    int file = open(RF.path,O_RDWR );

    if (file < 0) error("Unable to open file1.txt while sorting");

    char *tmp = malloc(sizeof(char)*RF.rec_size);
    char *key = malloc(sizeof(char)*RF.rec_size);

    int j;
    for(int i=1; i<RF.rec_number; i++) {
        key = get(RF,file,i);
        j = i-1;
        while(j>=0){ // j>=0 && tab[j] > key
            tmp = get(RF,file,j);
            if(tmp[0] > key[0]){

                //tab[j+1] = tab[j];
                set(RF,file, j+1,tmp);
                j--;
            }else{
                break;
            }
        }
        // tab[j+1] = key
        set(RF,file,j+1,key);
    }
    free(tmp);
    free(key);
    close(file);
}



void sort1(Records_File RF){
    FILE *file;
    file = fopen(RF.path,"r+");
    if(file == NULL) error("Cannot open a file");

    char *tmp = malloc(sizeof(char)*RF.rec_size);
    char *key = malloc(sizeof(char)*RF.rec_size);


    int j;
    for(int i=1; i<RF.rec_number; i++) {

        fseek(file,i*RF.rec_size,0); // key = get(RF,file,i);
        if(fread(key,1,RF.rec_size,file)!= RF.rec_size) error("Can't sort1");

        j = i-1;
        while(j>=0){ // j>=0 && tab[j] > key

            if(fseek(file,j*RF.rec_size,0) !=0) error("Can't sort2");

            if(fread(tmp,1,RF.rec_size,file)!=RF.rec_size) error("Can't sort3"); //tmp = get(RF,file,j);

            if(tmp[0] > key[0]){

                //tab[j+1] = tab[j];

                if(fseek(file,(j+1)*RF.rec_size,0) !=0) error ("Can't sort4");
                if(fwrite(tmp,1,RF.rec_size,file) != RF.rec_size) error("Can't sort5"); //tab[j+1] = tab[j];

                j--;
            }else{
                break;
            }
        }


        // tab[j+1] = key
        if(fseek(file,(j+1)*RF.rec_size,0) !=0) error("Can't sort6");
        if(fwrite(key,1,RF.rec_size,file)!=RF.rec_size) error("Can't sort7");

    }
    free(tmp);
    free(key);
    fclose(file);
}


void copy(Records_File RF){
    int src =open(RF.src_path,O_RDONLY);
    int dest = open(RF.dest_path,O_CREAT|O_RDWR|O_APPEND|O_TRUNC,0666);
    if(src <0  || dest <0) error("Cannot open files.");

    char * buff = malloc(RF.rec_size*sizeof(char));
    for(int i=0;i<RF.rec_number;i++){
        if(read(src,buff,RF.rec_size*sizeof(char)) <0) error("Can't read");
        if(write(dest,buff,RF.rec_size*sizeof(char))< 0 ) error("Can't write");
    }
    close(dest);
    close(src);

}

void copy1(Records_File RF){
    FILE * src =fopen(RF.src_path,"r");
    FILE * dest = fopen(RF.dest_path,"w+");
    if(src == NULL  || dest == NULL) error("Cannot open files.");
    if(strcmp(RF.src_path ,RF.dest_path)==0) error("Source file should be different than destination file ");

    char * buff = malloc(RF.rec_size*sizeof(char));
    for(int i=0;i<RF.rec_number;i++){
        if(fread(buff,1,RF.rec_size,src) <0) error("Can't read");
        if(fwrite(buff,1,RF.rec_size,dest)< 0 ) error("Can't write");
    }
    free(buff);
    fclose(dest);
    fclose(src);
}


void parse(int argc,char*argv[]){
    if(argc == 1){
            printf("g {file} {nr} {size} -  example: g file.txt 100 512  \n");
            printf("s {file} {nr} {size} {sys/lib}- example s file.txt 100 512 sys\n");
            printf("c {src_file} {dest_file} {nr} {size} {sys/lib}- example c file1.txt file2.txt 100 512 lib\n");
            printf(" nr - number of records, size - size of each record\n");
            printf("> {filename.txt}  save screen to file\n");
    }
    else if(argc == 5 && strcmp(argv[1],"g") ==0){
            Records_File RF = create(atoi(argv[3]),atoi(argv[4]),argv[2],NULL,NULL);
            Time T1 = measureTime();
            generate(RF);
            Time T2 = measureTime();
            printTimeDifference(T2,T1);
    }
    else if(argc == 6 && strcmp(argv[1],"s") ==0){

            Records_File RF = create(atoi(argv[3]),atoi(argv[4]),argv[2],NULL,NULL);
            if(strcmp(argv[5],"sys") ==0 ) {

                Time T1 = measureTime();
                sort(RF);
                Time T2 = measureTime();
                printTimeDifference(T2,T1);
            }else if(strcmp(argv[5],"lib") ==0 ){
                Time T1 = measureTime();
                sort1(RF);
                Time T2 = measureTime();
                printTimeDifference(T2,T1);

            }else{
                error("Bad args in sort command ");
            }
    }
    else if(argc == 7 && strcmp(argv[1],"c") ==0){
        //"c {src_file} {dest_file} {nr} {size} {sys/lib}- example c file1.txt file2.txt 100 512 lib\n")
            Records_File RF = create(atoi(argv[4]),atoi(argv[5]),NULL,argv[2],argv[3]);

            if(strcmp(argv[6],"sys") ==0 ){

                Time T1 = measureTime();
                copy(RF);
                Time T2 = measureTime();
                printTimeDifference(T2,T1);

            }else if(strcmp(argv[6],"lib")==0 ){
                Time T1 = measureTime();
                copy1(RF);
                Time T2 = measureTime();
                printTimeDifference(T2,T1);
            }else{
                error("Bad args in sort command ");
            }
    }
    else{
        error("Bad args");
    }
}



int main(char argc,char *argv[]) {

    parse(argc,argv);


    return 0;
}

