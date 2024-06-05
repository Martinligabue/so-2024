#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include "../include/common.h"
#include "../include/address_book.h"
#include "../include/auth.h"

void handle_client(int client_sock);
void process_command(int client_sock, Command *cmd);
void send_response(int client_sock, const char *response);

void signal_handler(int server_sock);

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);


    // Create socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_sock, 5) < 0)
    {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, signal_handler);
    printf("Server is running on port %d and pid %d ...\n", PORT, getpid());

    // Accept client connections
    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) > 0)
        handle_client(client_sock);

    close(server_sock);
    return 0;
}

void signal_handler(int server_sock) {
    printf("\nSignal received, closing socket...\n");
    close(server_sock);
    printf("Done! :)\n");
    exit(EXIT_SUCCESS);
}

void handle_client(int client_sock)
{
    Command cmd;
    ssize_t bytes_read;

    while ((bytes_read = read(client_sock, &cmd, sizeof(Command))) > 0)
        process_command(client_sock, &cmd);
}

void process_command(int client_sock, Command *cmd)
{
    printf("Received command: %d\n", cmd->type);
    printf("Received data: %s\n", cmd->data);

    printf("qui");
    char response[BUFFER_SIZE] = "";

    memset(response, '\0', sizeof(response));
    printf("%s", response);

    // check allowedclients.txt for pid
    int allowed = 0;

    FILE *fdallow = fopen("allowedClients.txt", "r");

    if (fdallow != NULL)
    {

        char line[256];
        while (fgets(line, sizeof(line), fdallow))
        {
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, cmd->pid) == 0)
            {
                allowed = 1;
                break;
            }
        }
        fclose(fdallow);
    }

    if ((cmd->type == CMD_ADD || cmd->type == CMD_MODIFY || cmd->type == CMD_DELETE) && !allowed)
    {
        strcpy(response, "Authentication required");
        send_response(client_sock, response);
        return;
    }
    // If we are here, we are authenticated, or we don't need to be
    switch (cmd->type)
    {
    case CMD_QUERY:
        query_address_book(cmd->data, response);
        break;
    case CMD_ADD:
        add_record(cmd->data, response);
        break;
    case CMD_MODIFY:
        modify_record(cmd->data, response);
        break;
    case CMD_DELETE:
        delete_record(cmd->data, response);
        break;
    case CMD_AUTH:
        if (authenticate(cmd->data))
        {
            FILE *fd;
            fd = fopen("allowedClients.txt", "a");
            fprintf(fd, "%s\n", cmd->pid);
            fclose(fd);
            strcpy(response, "Authentication successful");
        }
        else
            strcpy(response, "Authentication failed");
        break;
    case CMD_CLOSE:
        printf("Closing connection with ");
        break;
    default:
        strcpy(response, "Unknown command");
        break;
    }

    send_response(client_sock, response);
}

void send_response(int client_sock, const char *response)
{
    write(client_sock, response, strlen(response));
}
