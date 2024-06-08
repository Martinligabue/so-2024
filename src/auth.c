#include <string.h>
#include "../inc/auth.h"
#include <stdio.h>

/*
Function to remove newline character from a string.
*/
void remove_newline(char *str)
{
    str[strcspn(str, "\n")] = '\0';
}

/*
Function to check if credentials exist in the credentials file.
*/
int read_credentials_file(const char *credentials)
{
    FILE *fd = fopen(CREDENTIALS_FILE, "r");
    if (fd == NULL)
    {
        perror("Error opening credentials file");
        return 0;
    }

    // search in the file a line identical to credentials
    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), fd))
    {
        remove_newline(line);
        if (strcmp(line, credentials) == 0)
        {
            fclose(fd);
            return 1;
        }
    }

    fclose(fd);
    return 0;
}

int authenticate(const char *credentials)
{

    if (read_credentials_file(credentials))
    {
        printf("Authenticated\n");
        return 1;
    }
    printf("Not authenticated\n");
    return 0;
}
// eventualmente aggiungere ulteriori check, come segnare un user e una password, o addirittura un hash