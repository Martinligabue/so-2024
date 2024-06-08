#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../inc/common.h"

/*
Map user input to command type
*/
CommandType get_command_type(char *command_type)
{
    command_type[strcspn(command_type, "\n")] = 0; // remove \n from the string

    if (strcmp(command_type, "1") == 0)
        return CMD_QUERY;
    else if (strcmp(command_type, "2") == 0)
        return CMD_ADD;
    else if (strcmp(command_type, "3") == 0)
        return CMD_MODIFY;
    else if (strcmp(command_type, "4") == 0)
        return CMD_DELETE;
    else if (strcmp(command_type, "5") == 0)
        return CMD_AUTH;
    else if (strcmp(command_type, "6") == 0)
        return CMD_CLOSE;
    else
        return CMD_ERROR;
}

/*
Send a command to the server and read the response
*/
void send_command(int sock, Command *cmd, char *response)
{
    write(sock, cmd, sizeof(Command));
    read(sock, response, BUFFER_SIZE);
}

/*
Set up the socket
*/
void setup_socket(int *sock, struct sockaddr_in *server_addr)
{
    // Create socket
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Set up server address
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    if (inet_pton(AF_INET, ADDRESS, &server_addr->sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        close(*sock);
        exit(EXIT_FAILURE);
    }
}

/*
Connect to the server.
*/
void connect_to_server(int sock, struct sockaddr_in *server_addr)
{
    if (connect(sock, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
}

/*
Read user input with a prompt.
*/
void get_user_input(const char *prompt, char *input, size_t size)
{
    printf("%s", prompt);
    fgets(input, size, stdin);
    input[strcspn(input, "\n")] = 0; // remove \n from the string
}

/*
Handle user commands.
*/
void handle_commands(int sock)
{
    Command cmd;
    char response[BUFFER_SIZE];
    char command[20];

    snprintf(cmd.pid, sizeof(cmd.pid), "%d", getpid()); // Add self pid to cmd.pid struct

    do
    {
        printf(COMMAND_LIST);
        get_user_input("", command, sizeof(command));
        cmd.type = get_command_type(command);

        if (cmd.type != CMD_CLOSE && cmd.type != CMD_ERROR)
        {
            if (cmd.type == CMD_QUERY)
            {
                get_user_input(SUBQUERY, command, sizeof(command));
                strcpy(cmd.subinstruction, command);
            }

            char *prompt = (cmd.type == CMD_AUTH) ? REQUIRED_PASSWORD : REQUIRED_DATA;
            get_user_input(prompt, command, sizeof(command));
            strcpy(cmd.data, command);
        }

        send_command(sock, &cmd, response);
        printf("Server response: %s\n", response);
        memset(response, 0, BUFFER_SIZE);

    } while (cmd.type != CMD_CLOSE);

    close(sock);
    printf("Closed connection with no error\n");
}

int main()
{
    int sock;
    struct sockaddr_in server_addr;

    setup_socket(&sock, &server_addr);
    connect_to_server(sock, &server_addr);
    handle_commands(sock);

    return EXIT_SUCCESS;
}