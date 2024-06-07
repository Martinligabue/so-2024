#ifndef COMMON_H
#define COMMON_H

#define PORT 12345
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