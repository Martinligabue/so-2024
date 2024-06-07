#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../include/common.h"

void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char) str[i]);
    }
}

CommandType get_command_type(const char *command_type) {
    // Convert input to lowercase for case-insensitive comparison, then check string or number
    char lower_command_type[10];
    strncpy(lower_command_type, command_type, sizeof(lower_command_type) - 1);
    lower_command_type[sizeof(lower_command_type) - 1] = '\0';
    to_lowercase(lower_command_type);
    lower_command_type[strcspn(lower_command_type, "\n")] = 0; // remove \n from the string

    if (strcmp(lower_command_type, "query") == 0) {
        return CMD_QUERY;
    } else if (strcmp(lower_command_type, "add") == 0) {
        return CMD_ADD;
    } else if (strcmp(lower_command_type, "modify") == 0) {
        return CMD_MODIFY;
    } else if (strcmp(lower_command_type, "delete") == 0) {
        return CMD_DELETE;
    } else if (strcmp(lower_command_type, "auth") == 0) {
        return CMD_AUTH;
    } else if (strcmp(lower_command_type, "close") == 0) {
        return CMD_CLOSE;
    } else {
        return CMD_ERROR;
    }
}

void send_command(int sock, Command *cmd, char *response);

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char response[BUFFER_SIZE];
    Command cmd;
    // add self pid to cmd.pid struct
    snprintf(cmd.pid, sizeof(cmd.pid), "%d", getpid()); // why so difficult wtf

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Example: read command type from terminal
    char command[20];

    do {
        printf("Enter command type:\nQUERY\nADD\nMODIFY\nDELETE\nAUTH\nCLOSE\nEnter choice: ");
        fgets(command, sizeof(command), stdin);
        cmd.type = get_command_type(command);
        if (!(cmd.type == CMD_CLOSE || cmd.type == CMD_ERROR)) {
            if (cmd.type == CMD_QUERY) {
                printf("nome, cognome o matricola?: ");
                fgets(command, sizeof(command), stdin);
                command[strcspn(command, "\n")] = 0; // remove \n from the string
                strcpy(cmd.subinstruction, command);
            }
            printf("Enter command data: ");
            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = 0; // remove \n from the string
            strcpy(cmd.data, command);
        }
        send_command(sock, &cmd, response);

        printf("Server response: %s\n", response);
        memset(response, 0, BUFFER_SIZE);
    } while ((cmd.type != CMD_CLOSE) && (cmd.type != CMD_ERROR));
    close(sock);
    if (cmd.type == CMD_ERROR) {
        printf("Exited with invalid command type\n");
        exit(EXIT_FAILURE);
    }
    printf("Closed connection with no error\n");
    return EXIT_SUCCESS;
}

void send_command(int sock, Command *cmd, char *response) {
    write(sock, cmd, sizeof(Command));
    read(sock, response, BUFFER_SIZE);
}
