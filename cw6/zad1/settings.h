#ifndef ZAD1_SETTINGS_H
#define ZAD1_SETTINGS_H

#define PROJECT_ID 0xAF
#define MAXCLIENTS 128

typedef struct {
    long   type;       /* Message type. */
    char   text[100];    /* Message text. */
}Message;

typedef enum {MIRROR=1, CALC=2, TIME=3,END=4,HELLO=5 }Type;

#endif //ZAD1_SETTINGS_H
