#ifndef ZAD1_SETTINGS_H
#define ZAD1_SETTINGS_H

#define PROJECT_ID 0xAF
#define MAXCLIENTS 128
#define TEXT_SIZE 4096

typedef struct {
    long   type;       /* Message type. */
    char   text[TEXT_SIZE];    /* Message text. */
    int client_queue;
}Message;

typedef enum {MIRROR=1, CALC=2, TIME=3,END=4,HELLO=5 }Type;
const size_t MSG_SIZE = sizeof(Message) - sizeof(long);


#endif //ZAD1_SETTINGS_H
