#ifndef AUTH_H
#define AUTH_H

#define CREDENTIALS_FILE "res/passwords.txt"
#define ALLOWED_CLIENTS_FILE "log/allowedClients_log.txt"
#define BUFFER_SIZE 1024

int authenticate(char *credentials);

#endif
