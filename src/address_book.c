#include <stdio.h>
#include <string.h>
#include "../include/address_book.h"
#include "../include/common.h"

Record address_book[MAX_RECORDS];
int record_count = 1;

void query_address_book(const char *query, char *response)
{
    for (int i = 0; i < record_count; i++) {
        if (strcmp(address_book[i].name, query) == 0) {
            sprintf(response, "Query result: %s, %s, %s\n", address_book[i].name, address_book[i].address,
                    address_book[i].phone);
            return;
        }
        sprintf(response, "Record not found\n");
    }
}

void add_record(const char *record, char *response)
{
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
    record_count++;

    // Write the success message to response
    strcpy(response, "Record added");
}

void modify_record(const char *record, char *response)
{
    // Implementation for modifying a record in the address book
    strcpy(response, "Record modified");
}

void delete_record(const char *record, char *response)
{
    // Implementation for deleting a record from the address book
    strcpy(response, "Record deleted");
}
