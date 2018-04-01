#include "treewalk.h"
#define _GNU_SOURCE
#define _XOPEN_SOURCE 500

#include<ftw.h>
#include <dirent.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>


char *sign;
time_t date;

void error(char *s){
    printf("%s\n",s);
    perror("Program execution failed.");
    exit(EXIT_FAILURE);
}

time_t convertDateToTimeT(char* string) {
    struct tm time = {0, 2, 10, 1, 1, 2018};
    strptime(string, "%Y-%m-%d", &time);
    return mktime(&time);
}

char * convertDateToString(time_t time){
    char *buff = malloc(20);
    strftime(buff, 20, "%Y-%m-%d", localtime(&time));
    return buff;
}

int datediff(time_t time1,time_t time2){
    char *t1 = convertDateToString(time1);
    char *t2 = convertDateToString(time2);
    time1 = convertDateToTimeT(t1);
    time2 = convertDateToTimeT(t2);
    free(t1); free(t2);
    return difftime(time1,time2);
}

char* permissionsToString(mode_t perm) {
    char *result = calloc(10, sizeof(char));
    result[0] = (perm & S_IRUSR) ? 'r' : '-';
    result[1] = (perm & S_IWUSR) ? 'w' : '-';
    result[2] = (perm & S_IXUSR) ? 'x' : '-';
    result[3] = (perm & S_IRGRP) ? 'r' : '-';
    result[4] = (perm & S_IWGRP) ? 'w' : '-';
    result[5] = (perm & S_IXGRP) ? 'x' : '-';
    result[6] = (perm & S_IROTH) ? 'r' : '-';
    result[7] = (perm & S_IWOTH) ? 'w' : '-';
    result[8] = (perm & S_IXOTH) ? 'x' : '-';
    result[9] = '\0';
    return result;
}


void treeWalk( const char * path) {

    DIR * dir;

    if(dir = opendir(path)){
        struct dirent * dirent1;
        while(dirent1 = readdir(dir)){
            char * pathname = malloc(strlen(path)+strlen(dirent1->d_name)+2);
            strcpy(pathname,path);
            strcat(pathname,"/");
            strcat(pathname,dirent1->d_name);

            if( dirent1->d_type == DT_DIR && strcmp(dirent1->d_name,".")!=0  && strcmp(dirent1->d_name,"..")!=0){

                treeWalk(pathname);

            } else if(dirent1->d_type == DT_REG){
                struct stat stat1;
                if(stat(pathname,&stat1) == 0){
                    time_t modyfication_date  = stat1.st_mtime;

                    if(   (strcmp(sign,"e") == 0 && datediff(modyfication_date,date) < 0)
                          || (strcmp(sign,"=") == 0 && datediff(modyfication_date,date)== 0)
                          || (strcmp(sign,"l") == 0 && datediff(modyfication_date,date) > 0) ){

                        char *permissions = permissionsToString(stat1.st_mode);
                        char *string_modyfication_date=convertDateToString(modyfication_date);
                        printf("%s %zu %s %s\n",pathname,stat1.st_size,permissions,string_modyfication_date);
                        free(permissions);
                        free(string_modyfication_date);

                    }

                }

            }
            free(pathname);
        }
        closedir(dir);
    }
    return;
}


void printInfo(){
    printf("Give correct arguments:\n");
    printf("<path> <sign> <date> <version>\n");
    printf("<version> - should be 1 or 2\n");
    printf("<sign> may be '=', 'e', 'l'\n");
    printf("example: ./zad2 /home = 2018-03-26  2\n");
    exit(-1);
}


int function(const char *fpath, const struct stat *stat1, int typeflag, struct FTW *ftwbuf){
    if(typeflag == FTW_F ) {
        time_t modyfication_date = stat1->st_mtime;

        if ((strcmp(sign, "e") == 0 && datediff(modyfication_date, date) < 0)
            || (strcmp(sign, "=") == 0 && datediff(modyfication_date, date) == 0)
            || (strcmp(sign, "l") == 0 && datediff(modyfication_date, date) > 0)) {

            char *permissions = permissionsToString(stat1->st_mode);
            char *string_modyfication_date = convertDateToString(modyfication_date);
            printf("%s %zu %s %s\n", fpath, stat1->st_size, permissions, string_modyfication_date);
            free(permissions);
            free(string_modyfication_date);
        }
    }
    return 0;
}


void parse(int argc,char*argv[]){

    if(argc == 1) printInfo();
    if(argc!=5){

        printInfo();

    }
    if(!(strcmp(argv[2],"=")== 0 || strcmp(argv[2],"e")== 0 || strcmp(argv[2],"l")== 0)) printInfo();
    if(strlen(argv[2])!=1) printInfo();

    sign =malloc(2);
    strcpy(sign,argv[2]);
    date = convertDateToTimeT(argv[3]);

    char * path = malloc(100);
    realpath(argv[1], path);
    if(strcmp(argv[4],"1")==0){

        treeWalk(path);
        free(path);

    }else if(strcmp(argv[4],"2")==0){

        nftw(path,function,1,FTW_DEPTH|FTW_PHYS);

    }else{
        printInfo();
    }
    free(sign);

}