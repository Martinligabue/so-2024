#ifndef COMMON_H
#define COMMON_H

#define PORT 12345

#define EXTERNAL 0 // set this to 1 to connect to an external server
#if EXTERNAL == 1
#define ADDRESS "192.168.110.85" // change this to the server address
#else
#define ADDRESS "127.0.0.1"
#endif

#define BUFFER_SIZE 1024
#define MAX_RECORDS 100

#define COMMAND_LIST "Please select the command type:" \
                     "\n1: Queries the address book\n2: Add a new record (Name,Surname,StudentId)\n3: Edit a record (record to delete, new name, new surname, new studentId)\n4: Delete a record (given the id)\n5: Authenticate with a password\n6: Close this client\nYour selection: "

#define SUBQUERY "Perform the search for: \n1: First Name,\n2: Last Name,\n3: Student ID.\nYour selection: "

#define REQUIRED_DATA "Please enter the required data for the command: "
#define REQUIRED_PASSWORD "Please enter the password: "

typedef struct
{
    int id;
    char name[50];
    char surname[50];
    char studentId[7];
} Record;

typedef enum
{
    CMD_QUERY,
    CMD_ADD,
    CMD_MODIFY,
    CMD_DELETE,
    CMD_AUTH,
    CMD_CLOSE,
    CMD_ERROR
} CommandType;

typedef struct
{
    CommandType type;
    char data[BUFFER_SIZE];
    char subinstruction[10];
    char pid[10];
} Command;

#endif