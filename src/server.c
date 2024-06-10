#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include "../inc/common.h"
#include "../inc/address_book.h"
#include "../inc/auth.h"

/*Sends a response message to the client.*/
void send_response(int client_sock, const char *response)
{
    write(client_sock, response, strlen(response));
}

/*Signal handler for clean server shutdown.*/
void signal_handler(int server_sock)
{
    printf("\nSignal received, closing socket...\n");
    close(server_sock);
    printf("Saving address book\n");
    Command mockCmd;
    strcpy(mockCmd.subinstruction, "4");
    char mockResponse[BUFFER_SIZE] = {0};
    query_address_book(mockCmd.data, mockCmd.subinstruction, mockResponse);

    FILE *file = fopen("res/new_address_book.csv", "w"); // truncates the file to 0, then writes to it
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (fprintf(file, "%s", mockResponse) < 0)
    {
        perror("Error writing to file");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);

    printf("Done! :)\n");
    exit(EXIT_SUCCESS);
}

/*Checks if a client with a given PID is allowed by reading from allowedClients.txt.
Returns 1 if allowed, 0 otherwise.*/
int is_client_allowed(char *pid)
{
    FILE *fdAllowedClients = fopen(ALLOWED_CLIENTS_FILE, "r");
    if (fdAllowedClients == NULL)
    {
        perror("Error opening allowedClients.txt");
        return EXIT_FAILURE;
    }

    char line[256];
    int allowed = 0;
    while (fgets(line, sizeof(line), fdAllowedClients))
    {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, pid) == 0)
        {
            allowed = 1;
            break;
        }
    }
    fclose(fdAllowedClients);
    return allowed;
}

/*Processes a single command from the client and sends appropriate responses to the client.*/
void process_command(int client_sock, Command *cmd)
{
    printf("Received command: %d\n", cmd->type);
    printf("Received data: %s\n", cmd->data);

    char response[BUFFER_SIZE] = {0};

    // check allowedclients.txt for pid
    fflush(stdout);
    if ((cmd->type == CMD_ADD || cmd->type == CMD_MODIFY || cmd->type == CMD_DELETE))
    {
        if (is_client_allowed(cmd->pid) == 0) // se non ha trovato il pid, è ancora 0
        {
            send_response(client_sock, "Authentication required\n");
            return;
        }
    }
    // If we are here, we are authenticated, or we don't need to be
    switch (cmd->type)
    {
    case CMD_QUERY:
        query_address_book(cmd->data, cmd->subinstruction, response);
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
            FILE *fd = fopen(ALLOWED_CLIENTS_FILE, "a");
            if (fd == NULL)
            {
                perror("Error opening file!\n");
                return;
            }
            fprintf(fd, "%s\n", cmd->pid);
            fclose(fd);
            send_response(client_sock, "Authentication successful\n");
        }
        else
            strcpy(response, "Authentication failed\n");
        break;
    case CMD_CLOSE:
        printf("Closing connection with %s.\n", cmd->pid);
        memset(response, 0, sizeof(response));
        send_response(client_sock, "Connection closed cleanly\n");
        break;
    default:
        strcpy(response, "Invalid command.\nPlease enter the correct number.\n");
        break;
    }
    send_response(client_sock, response);
    memset(response, 0, sizeof(response));
}

/*Handles communication with a single client */
void handle_client(int client_sock)
{
    Command cmd;
    ssize_t bytes_read;

    while ((bytes_read = read(client_sock, &cmd, sizeof(Command))) > 0)
        process_command(client_sock, &cmd);
}

/* Sets up the server socket and binds it to the specified address and port.*/
void setup_server(int *server_sock, struct sockaddr_in *server_addr)
{
    if ((*server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);

    if (bind(*server_sock, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0)
    {
        perror("Bind failed");
        close(*server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(*server_sock, 5) < 0)
    {
        perror("Listen failed");
        close(*server_sock);
        exit(EXIT_FAILURE);
    }
}

/*Accepts client connections in a loop.*/
void accept_client_connections(int server_sock)
{
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);

        if (client_sock == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // No more incoming connections for now, sleep for a while and then continue the loop
                sleep(1);
                continue;
            }
            else
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }

        handle_client(client_sock);
    }
}
int set_socket_non_blocking(int socket_fd)
{
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(socket_fd, F_SETFL, flags) == -1)
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}
int main()
{
    int server_sock;
    struct sockaddr_in server_addr;

    addDefaults(); // Add default records to the address book

    setup_server(&server_sock, &server_addr);
    int result = set_socket_non_blocking(server_sock);
    printf("result: %d\n", result);
    if (result == -1)
    {
        perror("Error setting socket to non-blocking");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, signal_handler);
    printf("Server is running on %s:%d and pid %d ...\n", ADDRESS, PORT, getpid());

    accept_client_connections(server_sock);

    close(server_sock);
    return 0;
}
