#include <stdio.h>
#include <string.h>
#include "../include/address_book.h"
#include "../include/common.h"

Record address_book[MAX_RECORDS];
int record_count = 0;

void query_address_book(const char *query, char *response)
{
    // search the address book for the query
    for (int i = 0; i < record_count; i++)
    {
        if (strcmp(address_book[i].name, query) == 0)
        {
            char buffer[256]; // Ensure the buffer is large enough
            sprintf(buffer, "%s, %s, %s\n", address_book[i].name, address_book[i].address, address_book[i].phone);
            strcpy(response, buffer);
            return;
        }
    }
    strcpy(response, "Query result");
}

void add_record(const char *record, char *response)
{
    // Implementation for adding a record to the address book
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
