#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


int main() {
    const char *str = "Into my handler\n";
    write(1, str, strlen(str));

}