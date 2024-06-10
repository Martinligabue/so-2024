#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <signal.h>
#include "../inc/common.h"
#include "../inc/address_book.h"
#include "../inc/auth.h"
#include <sys/select.h>

#define MAX_CLIENTS 10

int main()
{
    int server_sock, client_sock, max_clients = MAX_CLIENTS;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sockets[max_clients];
    fd_set read_fds, active_fds;
    Command cmd;
    ssize_t bytes_read;

    addDefaults(); // Add default records to the address book

    // Create socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket to non-blocking mode
    if (fcntl(server_sock, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("Failed to set server socket to non-blocking mode");
        close(server_sock);
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

    printf("Server is running on %s:%d and pid %d ...\n", ADDRESS, PORT, getpid());

    // Initialize client sockets array
    for (int i = 0; i < max_clients; i++)
    {
        client_sockets[i] = -1;
    }

    FD_ZERO(&active_fds);
    FD_SET(server_sock, &active_fds);

    while (1)
    {
        read_fds = active_fds;

        // Wait for activity on any socket
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0)
        {
            perror("Select failed");
            break;
        }

        // Check if there is a new connection
        if (FD_ISSET(server_sock, &read_fds))
        {
            client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
            if (client_sock < 0)
            {
                perror("Accept failed");
                continue;
            }

            // Add new client socket to the array
            for (int i = 0; i < max_clients; i++)
            {
                if (client_sockets[i] == -1)
                {
                    client_sockets[i] = client_sock;
                    break;
                }
            }

            // Add new client socket to the active set
            FD_SET(client_sock, &active_fds);

            // Set new client socket to non-blocking mode
            if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
            {
                perror("Failed to set client socket to non-blocking mode");
                close(client_sock);
                continue;
            }

            printf("New connection, socket fd is %d, ip is : %s, port : %d\n", client_sock, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }

        // Handle data from clients
        for (int i = 0; i < max_clients; i++)
        {
            if (client_sockets[i] != -1 && FD_ISSET(client_sockets[i], &read_fds))
            {
                bytes_read = read(client_sockets[i], &cmd, sizeof(Command));
                if (bytes_read > 0)
                {
                    process_command(client_sockets[i], &cmd);
                }
                else if (bytes_read == 0)
                {
                    // Connection closed by client
                    printf("Client disconnected, socket fd is %d\n", client_sockets[i]);
                    close(client_sockets[i]);
                    FD_CLR(client_sockets[i], &active_fds);
                    client_sockets[i] = -1;
                }
                else
                {
                    // Error reading from client
                    perror("Read failed");
                    close(client_sockets[i]);
                    FD_CLR(client_sockets[i], &active_fds);
                    client_sockets[i] = -1;
                }
            }
        }
    }

    // Close all client sockets
    for (int i = 0; i < max_clients; i++)
    {
        if (client_sockets[i] != -1)
        {
            close(client_sockets[i]);
        }
    }

    close(server_sock);
    return 0;
}
