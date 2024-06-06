#include <stdio.h>
#include <string.h>
#include "../include/address_book.h"
#include "../include/common.h"

Record address_book[MAX_RECORDS];
int record_count = 1;
int id = 1;

int query_address_book(const char *query, char *response) {
    for (int i = 0; i < record_count; i++) {
        if (strcmp(address_book[i].name, query) == 0) {
            sprintf(response, "Query result: %s, %s, %s\n", address_book[i].name, address_book[i].address,
                    address_book[i].phone);
            return address_book[i].id;
        }
        sprintf(response, "Record not found\n");
    }
    return -1;
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

    // Add the new record to the address book
    strcpy(address_book[record_count].name, name);
    strcpy(address_book[record_count].address, address);
    strcpy(address_book[record_count].phone, phone);
    address_book[record_count].id = id++;
    record_count++;

    // Write the success message to response
    strcpy(response, "Record added");
}

void delete_record(const char *record, char *response) {
    int del = query_address_book(record, response);
    if (del == -1) {
        strcpy(response, "Record not found");
        return;
    }
    record_count--;
    address_book->id = 0;
    address_book->phone[0] = '\0';
    address_book->address[0] = '\0';
    address_book->name[0] = '\0';

    strcpy(response, "Record deleted");
}

void modify_record(const char *record, char *response) {

    strcpy(response, "Record modified");
}
