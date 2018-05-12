#ifndef ZAD1_SETTINGS_H
#define ZAD1_SETTINGS_H

#define PROJECT_ID 0xAF
#define MAXCLIENTS 128
#define TEXT_SIZE 254

typedef struct {
    long type;       /* Message type. */
    char text[TEXT_SIZE];    /* Message text. */
    pid_t pid;
} Message;

typedef enum {
    MIRROR = 1, CALC = 2, TIME = 3, END = 4, HELLO = 5, STOP = 6
} Type;


#endif //ZAD1_SETTINGS_H
