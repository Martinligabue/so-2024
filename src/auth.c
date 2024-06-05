#include <string.h>
#include "../include/auth.h"
#include <stdio.h>

int authenticate(const char *credentials) {
    // Simple authentication check
    //return strcmp(credentials, "password") == 0;

    FILE *fd;
    fd = fopen("res/passwords.txt", "r");
    //search in the file a line identical to credentials
    char line[256];
    while(fgets(line, sizeof(line), fd)){
        //remove the newline character from the line
        line[strcspn(line, "\n")] = '\0';
        if(strcmp(line, credentials) == 0){
            fclose(fd);
            printf("Authenticated\n");
            return 1;
        }
    }
    fclose(fd);
    printf("Not authenticated\n");
    return 0;
}
//eventualmente aggiungere ulteriori check, come segnare un user e una password, o addirittura un hash
