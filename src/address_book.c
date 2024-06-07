#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/address_book.h"
#include "../include/common.h"

Record address_book[MAX_RECORDS];
int record_count = 0;
int id = 1;

void concatenateResults(char *buffer, char *buffer2, int i, Record *address_book) {
    sprintf(buffer2, "%d: %s, %s, %s\n", address_book[i].id, address_book[i].name, address_book[i].address,
            address_book[i].phone);
    strcat(buffer, buffer2);
    return;
}

void query_address_book(char *query, char *sub, char *response) {
    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    char querycopy[BUFFER_SIZE];
    memset(response, 0, BUFFER_SIZE);

    strcpy(querycopy, query);
    querycopy[strcspn(querycopy, "\n")] = 0; // remove \n from the string
    sub[strcspn(sub, "\n")] = 0; // remove \n from the string
    for (int i = 0; i <= record_count; i++) {
        if (strcmp(sub, "nome") == 0) {
            if (strcmp(address_book[i].name, querycopy) == 0)
                concatenateResults(buffer, buffer2, i, address_book);
        } else if (strcmp(sub, "cognome") == 0) {
            if (strcmp(address_book[i].address, querycopy) == 0)
                concatenateResults(buffer, buffer2, i, address_book);
        } else if (strcmp(sub, "matricola") == 0) {
            if (strcmp(address_book[i].phone, querycopy) == 0)
                concatenateResults(buffer, buffer2, i, address_book);
        }/* else if (strcmp(sub, "tutto") == 0) {
            concatenateResults(buffer, buffer2, i, address_book);
        } */else {
            sprintf(response, "Invalid subinstruction\n");
            return;
        }
    }
    if (strcmp(buffer, "") == 0) {
        sprintf(response, "Record not found\n");
        return;
    }
    memset(response, 0, BUFFER_SIZE);
    sprintf(response, "\n%s", buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(buffer2, 0, sizeof(buffer2));
    memset(querycopy, 0, sizeof(querycopy));

    return;
}

void add_record(const char *record, char *response) {
    // Check if the address book is full
    if (record_count >= MAX_RECORDS) {
        strcpy(response, "Address book is full");
        return;
    }

    // Parse the record string
    char *record_copy = strdup(record); // Make a copy of the record string because strtok modifies the string
    char *name = strtok(record_copy, ",");
    char *address = strtok(NULL, ",");
    char *phone = strtok(NULL, ",");

    // Check if all fields were provided
    if (name == NULL || address == NULL || phone == NULL) {
        strcpy(response, "Invalid record format");
        return;
    }
    //check that phone is made of only numbers
    for (int i = 0; i < strlen(phone); i++) {
        if (phone[i] < '0' || phone[i] > '9') {
            strcpy(response, "Invalid student Id");
            return;
        }
    }

    // Add the new record to the address book
    strcpy(address_book[record_count].name, name);
    strcpy(address_book[record_count].address, address);
    strcpy(address_book[record_count].phone, phone);
    address_book[record_count].id = id++;
    record_count++;

    // Write the success message to response
    strcpy(response, "Record added");
}

void delete_record(const char *recordId, char *response) {
    int del = atoi(recordId);
    if (del > record_count || del <= 1) {
        strcpy(response, "Record doesn't exist\n");
        return;
    }
    record_count--;
    address_book[del - 1].id = 0;
    address_book[del - 1].name[0] = '\0';
    address_book[del - 1].address[0] = '\0';
    address_book[del - 1].phone[0] = '\0';
    strcpy(response, "Record deleted");
}

void modify_record(const char *record, char *response) {

    // Parse the record string
    char *record_copy = strdup(record); // Make a copy of the record string because strtok modifies the string
    char *id = strtok(record_copy, ",");
    char *name = strtok(NULL, ",");
    char *address = strtok(NULL, ",");
    char *phone = strtok(NULL, ",");

    // Check if all fields were provided
    if (name == NULL || address == NULL || phone == NULL) {
        strcpy(response, "Invalid record format");
        return;
    }
    int idn = atoi(id);
    if (idn < 0 || idn > record_count) {
        strcpy(response, "Record doesn't exist\n");
        return;
    }

    // Add the new record to the address book
    address_book[idn - 1].id = idn;
    strcpy(address_book[idn - 1].name, name);
    strcpy(address_book[idn - 1].address, address);
    strcpy(address_book[idn - 1].phone, phone);
    strcpy(response, "Record modified");
}

void addDefaults() {

    FILE *fd;
    fd = fopen("res/rubrica.csv", "r");
    if (fd == NULL) {
        perror("Errore nell'apertura del file!\n");
        return;
    }

    // search in the file a line identical to credentials
    char line[256];
    while (fgets(line, sizeof(line), fd)) {
        line[strcspn(line, "\n")] = '\0';
        char *line_copy = strdup(line); // Make a copy of the record string because strtok modifies the string
        char *name = strtok(line_copy, ",");
        char *address = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        strcpy(address_book[record_count].name, name);
        strcpy(address_book[record_count].address, address);
        strcpy(address_book[record_count].phone, phone);
        address_book[record_count].id = id++;
        record_count++;
    }
    fclose(fd);
}
