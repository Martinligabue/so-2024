#ifndef COMMON_H
#define COMMON_H

#define PORT 12345

#define EXTERNAL 1
#if EXTERNAL==1
#define ADDRESS "192.168.110.85" // change this to the server address
#else
#define ADDRESS "127.0.0.1"
#endif

#define BUFFER_SIZE 1024
#define MAX_RECORDS 100

typedef struct {
    int id;
    char name[50];
    char phone[15];
    char address[100];
} Record;

typedef enum {
    CMD_QUERY,
    CMD_ADD,
    CMD_MODIFY,
    CMD_DELETE,
    CMD_AUTH,
    CMD_CLOSE,
    CMD_ERROR
} CommandType;


typedef struct {
    CommandType type;
    char data[BUFFER_SIZE];
    char subinstruction[10];
    char pid[10];
} Command;

#endif